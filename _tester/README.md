# _tester — Harnais de stress webserv

Testeur de fiabilité pour l'évaluation 42. Cible **Ubuntu** (utilise `valgrind`,
`/proc`, `ss`). Prouve que le serveur **ne crash pas, ne fuit pas (mémoire + FD),
et tient la charge** (disponibilité ≥ 99.5 %).

## Prérequis (Ubuntu / 42)

```bash
sudo apt install -y valgrind siege
# python3 est déjà là
```

## Lancer

Depuis n'importe où :

```bash
./_tester/run_stress.sh              # passe valgrind (légère) + passe native (lourde)
./_tester/run_stress.sh --valgrind-only
./_tester/run_stress.sh --native-only
./_tester/run_stress.sh --quick      # sièges courts, pour itérer en dev
```

Le script **compile** (`make re`), lance le serveur avec
[`config/webserv_test.conf`](config/webserv_test.conf) sur le port **8080**,
joue tous les tests, puis rend un **verdict** + code retour (`0` = tout vert).

Surcharges possibles :

```bash
PORT=8080 SIEGE_TIME=2M SIEGE_CONC=100 ./_tester/run_stress.sh --native-only
CONF=doc/config_file ./_tester/run_stress.sh      # tester avec ta vraie config
```

## Ce qui est vérifié

### Passe valgrind (`run_stress.sh`, charge légère)
Serveur lancé sous :

```
valgrind --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all \
         --track-fds=yes --trace-children=no --child-silent-after-fork=yes \
         --error-exitcode=42
```

- `definitely / indirectly lost` → **échec** si > 0
- `possibly lost / still reachable` → avertissement (souvent globals/STL)
- `ERROR SUMMARY` (invalid read/write, uninit, double free) → **échec** si > 0
- **`--track-fds=yes`** : liste les FD encore ouverts à la sortie. Plus de 3
  (stdin/stdout/stderr) = **socket ou pipe CGI non fermé** → échec.
- Arrêt **propre par SIGINT** : indispensable pour que valgrind rende son rapport.
- `--trace-children=no` : on ne suit **pas** les CGI (`execve` python/php), hors sujet.

### Passe native (`run_stress.sh`, charge lourde)
- `siege -b -c 25 -t 1M` → **disponibilité ≥ 99.5 %** (seuil éval)
- Échantillonnage **RSS** (`/proc/PID/status`) : le RSS final ne doit pas
  dépasser 150 % de l'initial → sinon fuite sous charge.
- Échantillonnage **nombre de FD** (`/proc/PID/fd`) : doit revenir au niveau de
  base après la charge → sinon FD non fermés.
- **CLOSE-WAIT** (`ss -tan`) : > 5 connexions pendantes = sockets non fermées.
- Rejeu des cas-limites **après** la charge (détection de dégradation).
- Le serveur doit rester **vivant sans relance** sur toute la passe.

### Cas-limites en sockets bruts ([`stress_test.py`](stress_test.py))
Ce que `siege`/`curl` ne savent pas faire. Chaque test vérifie qu'après
l'agression le serveur **répond encore** (liveness) :

- requête vide / ligne malformée / méthode & version inconnues / Host manquant
- **slowloris** (envoi octet par octet)
- **déconnexion brutale** mi-requête (RST) × 50 → SIGPIPE / FD leak
- header géant (64 Ko) / 2000 headers → épuisement parse/mémoire
- body > `client_max_body_size` → **413**
- `Content-Length` menteur / négatif / dupliqué
- **chunked** valide / cassé / taille non-hexa
- **pipelining** (keep-alive)
- **flood** de 200 connexions muettes → épuisement des FD
- **600 requêtes concurrentes** (20 threads) → races / double free
- **CGI `.py` × 30** en rafale → fuites de pipes / zombies

Peut aussi se lancer seul :

```bash
python3 _tester/stress_test.py 127.0.0.1 8080
```

## Logs

Écrits dans `_tester/logs/` (ignoré par git) : `valgrind.log`, `server.log`,
`siege.log`. Toujours relire le `valgrind.log` complet en cas d'échec.

## Notes

- Le serveur webserv utilise **epoll** : il ne compile **que sous Linux**. Ce
  harnais est fait pour tourner sur la machine d'éval (Ubuntu), pas sur macOS.
- Le port du test (8080) doit correspondre à celui de la config utilisée.
