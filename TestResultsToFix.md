# Tests à corriger

Ce fichier liste les correctifs à apporter, sous forme de commits distincts,
pour faire passer les tests de `tests/unittest_python/` qui sont nécessaires
pour la correction (comportement HTTP/CGI/chunked/keep-alive de base).
Les tests non listés ici (ex: `DELETE /` qui supprimerait `www/index.html`,
un test dont le contenu de fichier ne correspond pas exactement) sont
volontairement exclus car pas nécessaires à la correction.

Tout le code correctif a déjà été écrit puis discard sur demande — ce fichier
sert de plan pour le refaire proprement, commit par commit.

---

## Commit 1 — Implémenter `Connection: keep-alive` / `Connection: close`

**Bug** : `AResponse::makeHeader()` (`src/response/AResponse.cpp`) n'émet
jamais de header `Connection`, et le socket n'est jamais fermé après une
réponse. `Body::_keepAlive` est bien parsé depuis la requête
(`src/http/Body.cpp`, `configureKeepAlive`) mais n'est lu nulle part —
code mort. Résultat : la connexion reste toujours ouverte, même après une
erreur ou une demande explicite de `Connection: close`.

**Tests concernés** :
- `Send Keep-Alive` (`test_keepAlive_close.py::test_keep_alive`)
- `Send Close connection` (`test_keepAlive_close.py::test_connection_close_request`)
- `Same client Keep Alive 2 times` (`test_keepAlive_close.py::test_keep_alive_both_requests`)
- `A malformed chunk closes the connection even without Connection: close` (`test_ChunkErrorRecovery.py`)
- Et indirectement, plusieurs tests qui timeoutaient parce que le client
  attend une fermeture de socket qui n'arrivait jamais (voir Commit 2).

**Correctif** :
1. `Body::_keepAlive` doit défaut à `true` (persistant par défaut en
   HTTP/1.1), et `configureKeepAlive` ne doit passer à `false` que si
   `Connection: close` est explicitement reçu (pas l'inverse).
2. Dans `AResponse::makeHeader()`, calculer une décision `closeConnection` :
   - `true` si `statusCode >= 400` (une erreur laisse le parsing dans un état
     indéterminé, la connexion ne peut plus être réutilisée en sécurité) ;
   - sinon `!body->getKeepAlive()` (avec garde NULL si `HttpRequest`/`Body`
     n'existe pas encore, ex. erreur avant parsing du body).
   - Ajouter le header `Connection: close` ou `Connection: keep-alive` en
     conséquence (attention : les autres headers de ce codebase sont
     sérialisés sans espace après `:`, mais les tests attendent
     `"Connection: close"` avec l'espace — mettre l'espace uniquement pour
     ce header).
3. Stocker cette décision quelque part de persistant après
   `AResponse` (les objets `AResponse` sont locaux à
   `HttpResponse::initialisationHttpResponse()`) — ajouter un champ
   `_shouldCloseConnection` + getter/setter sur `HttpResponse`.
4. Dans `Webserv::sendResponseToClient()` (`src/core/Webserv.cpp`), après
   `response.sendToClient()`, si `response.getShouldCloseConnection()` est
   vrai, fermer réellement la connexion (`handleDisconnect(client)`), sinon
   ne rien faire. Gérer aussi le cas `client->isPendingDelete()` (CGI qui se
   termine après une déconnexion) en premier.
5. Simplifier `Webserv::readToChild()` qui faisait déjà un
   `if (isPendingDelete()) deleteClient()` en double — ce sera géré par
   `sendResponseToClient()` désormais.

---

## Commit 2 — Ne pas bloquer indéfiniment sur un body déclaré trop gros

**Bug** : `Webserv::readAndCheckRequestCompletion()` /
`isCompleteRequest()` (`src/http/utilsRequest.cpp`) attendent que TOUT le
body déclaré (`Content-Length` ou taille de chunk) soit arrivé avant de
laisser la requête être traitée. Si un client déclare
`Content-Length: 100000` (ou une taille de chunk énorme) et n'envoie que
quelques octets puis s'arrête, le serveur attend indéfiniment — alors que
`Body::parseContentLengthBody`/`parseChunkedBody` savent déjà rejeter avec
413 dès que la taille déclarée dépasse `client_max_body_size`, **avant**
d'avoir besoin du body complet (l'ordre des checks dans ces fonctions place
déjà le test de taille avant le test de complétude).

**Tests concernés** :
- `Oversized chunk size is rejected without waiting body` (`test_ChunkedMaxBodySize.py`)
- `413 on Content-Length over the limit without sending the full body` (`test_InvalidContentType.py`)
- (Sans ce fix, ces deux tests timeout après 3s — ce n'est pas juste un
  mauvais code retour, c'est un vrai risque de blocage du serveur.)

**Correctif** :
1. Ajouter `Webserv::largestConfiguredMaxBodySize()` : parcourt tous les
   `Server`/`Location` de la config et retourne le plus grand
   `client_max_body_size` configuré n'importe où (0 si aucun n'est défini
   → pas de raccourci possible).
2. Ajouter `isDeclaredBodySizeExceeding(request, maxBodySize)` dans
   `utilsRequest.cpp` : une fois les headers complets (`\r\n\r\n` trouvé),
   regarde `Content-Length` (ou la taille du premier chunk si
   `Transfer-Encoding: chunked`) et retourne `true` si elle dépasse
   `maxBodySize`.
3. Dans `readAndCheckRequestCompletion`, si `isCompleteRequest()` est faux
   mais `isDeclaredBodySizeExceeding(...)` est vrai, retourner
   `REQUEST_COMPLETE` quand même — ça laisse le pipeline normal traiter la
   requête et rejeter avec 413 sans avoir bufferisé tout le body.

Note : cette approche utilise le plus grand `client_max_body_size` de toute
la config (pas celui de la location précise), car résoudre la location
exacte nécessiterait du parsing de headers/routing avant que le body ne
soit là. C'est une approximation sûre : si la taille déclarée dépasse le
maximum configuré *n'importe où*, elle dépasse forcément aussi celui de la
location réelle.

---

## Commit 3 — 404 sur les chemins inexistants sous une location

**Bug** : `HandlePath::createPathWithLocation()` /
`createPathWithServer()` (`src/request/HandlePath.cpp`) retombent sur
l'index de la location (ou celui du serveur) dès qu'un fichier exact n'est
pas trouvé — **même si l'URI ne correspond pas du tout à la racine de la
location**. Résultat : `GET /nimportequoi` sert `index.html` (200) au lieu
de 404, pareil pour `POST`/`DELETE` sur un chemin qui n'existe pas.

**Tests concernés** :
- `Test method GET on something not existing` → attend 404, reçoit 200
- `Test method POST on something not existing` → attend 404, reçoit 200
- `Invalid hexadecimal chunk size` (`test_invalid_transferEncoding.py`) →
  attend 404 (le fichier ne doit pas être créé via CGI), reçoit 200
- `Invalid value received chunk value` → idem

**Correctif** :
1. Ajouter `HandlePath::isRequestForLocationRoot(locationName)` : compare
   l'URI (`getHeader()->getScriptName()`) au nom de la location, en
   tolérant un `/` final des deux côtés.
2. Dans `createPathWithLocation()` :
   - Pour `POST` : avant de faire `return pathLoc;`, vérifier
     `isRequestForLocationRoot(location->getName())` — sinon `throw
     std::runtime_error("404")`.
   - Après le check `resolveRequestedFilePath` (qui gère déjà le cas
     "fichier exact existant"), ajouter le même check avant le bloc `if
     (method == GET) { ...index/autoindex... }` — sinon 404 direct plutôt
     que de retomber sur `createPathWithServer()`.

Ne **pas** toucher `createPathWithServer()` elle-même : elle n'est
atteinte, après ce fix, que quand l'URI correspond déjà à la racine de la
location (donc son propre comportement de fallback reste correct et sans
risque de régression).

---

## Commit 4 — Codes de statut CGI par défaut (201/204)

**Bug** : `ResponseContext::_statusCode` vaut toujours `200` par défaut, et
rien ne le change pour les requêtes CGI. Le script `www/cgi-bin/router.py`
n'émet jamais de header `Status:` (volontairement, cf. son propre
commentaire). `CGIRequest::forwardCgiHeaders()`
(`src/request/CGIRequest.cpp`) ne traite que `Set-Cookie`, ignore tout le
reste — y compris un éventuel header `Status:` du CGI.

**Tests concernés** :
- `Test CGI POST create a file` → attend 201, reçoit 200
- `Test CGI DELETE remove a file` → attend 204, reçoit 200
- `Transfer Encoding CGI test if valid` → attend 201, reçoit 200
- Cascade : `Test retrieving the newly created file via GET` dépend du bon
  déroulement du test POST précédent.

**Correctif** dans `CGIRequest::forwardCgiHeaders()` :
1. Parser un éventuel header `Status: <code>` renvoyé par le CGI et
   appeler `getResponseContext()->setStatusCode(...)` en conséquence
   (conforme CGI/1.1, RFC 3875).
2. Si le CGI n'a rien précisé, appliquer une convention REST par défaut
   selon la méthode HTTP d'origine
   (`getRequestContext()->getHttpRequest()->getHeader()->getMethod()`) :
   `POST` → 201, `DELETE` → 204, sinon laisser 200.

---

## Non traité / à investiguer séparément (pas requis pour la correction)

- **`Test method DELETE on /` attend 200** → tracé jusqu'à : il faudrait
  que `DELETE /` supprime `www/index.html` (fichier non versionné dans
  git, donc pas de backup). Décision prise : ne pas implémenter, le test
  reste en échec. Si on veut le corriger un jour : soit sauvegarder
  `index.html` avant, soit changer l'attente du test à 404.
- **`Test retrieving the newly created file via GET`** : après le Commit 4,
  il ne reste qu'un écart d'un `\n` final entre le contenu attendu et reçu
  (`'...post.py\n' != '...post.py'`). Pas totalement diagnostiqué — un test
  manuel avec `nc` sur un petit body (5 octets) ne reproduisait pas l'écart,
  donc c'est peut-être lié à la taille du body ou à un découpage en
  plusieurs `recv()`. À creuser si ce test compte pour la correction.
- **SIGSEGV Valgrind au shutdown** (`Server::~Server()` →
  `Webserv::getEpollFd()`, accès mémoire invalide) : préexistant, présent
  même avant toute modification de cette session (`ERROR SUMMARY: 2 errors
  from 2 contexts` dès le tout premier `make test`). Pas lié aux commits
  ci-dessus.
- **Use-after-free détecté dans `checkTimeOut()`** (travail en cours,
  fonction pas encore committée) : Valgrind montre des lectures invalides
  sur des `EventData*` déjà libérés par `CGIRequest::~CGIRequest()` — le
  set `_setEventData` garde apparemment des pointeurs qui ne sont plus
  retirés avant que l'objet pointé ne soit détruit. À corriger quand ce
  travail sera repris (actuellement `git stash`é, message
  `discarded-fixes-see-TestResultsToFix`).
