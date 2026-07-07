#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Usage:
    python3 stress_test.py [host] [port]
    python3 stress_test.py 127.0.0.1 8080
"""

import socket
import sys
import time
import threading

HOST = sys.argv[1] if len(sys.argv) > 1 else "127.0.0.1"
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 8080

# Timeout | if bigger than macro, it crashes.
TIMEOUT = 5.0

GREEN = "\033[32m"
RED = "\033[31m"
YELLOW = "\033[33m"
CYAN = "\033[36m"
RESET = "\033[0m"

results = []  # (name, ok, detail)


def log_pass(name, detail=""):
    results.append((name, True, detail))
    print("  {}[PASS]{} {} {}".format(GREEN, RESET, name, detail))


def log_fail(name, detail=""):
    results.append((name, False, detail))
    print("  {}[FAIL]{} {} {}".format(RED, RESET, name, detail))


def connect():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(TIMEOUT)
    s.connect((HOST, PORT))
    return s


def send_raw(payload, read=True, close_after=True, delay_close=0.0):
    """SEND payload (bytes) | READ response | SEND received bites"""
    s = connect()
    try:
        s.sendall(payload)
        data = b""
        if read:
            try:
                while True:
                    chunk = s.recv(4096)
                    if not chunk:
                        break
                    data += chunk
            except socket.timeout:
                pass
        if delay_close:
            time.sleep(delay_close)
        return data
    finally:
        if close_after:
            try:
                s.close()
            except OSError:
                pass


def status_code(response):
    if not response:
        return None
    try:
        line = response.split(b"\r\n", 1)[0].decode("latin-1")
        parts = line.split()
        if len(parts) >= 2 and parts[0].startswith("HTTP/"):
            return int(parts[1])
    except (ValueError, IndexError):
        return None
    return None


def is_alive():
    """Check if server still alive after success"""
    try:
        r = send_raw(b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
        return status_code(r) is not None
    except OSError:
        return False


# ===========================================================================
#  TESTS
# ===========================================================================

def t_basic_get():
    r = send_raw(b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
    code = status_code(r)
    if code == 200:
        log_pass("GET / -> 200")
    else:
        log_fail("GET / -> 200", "recu code={}".format(code))


def t_not_found():
    r = send_raw(b"GET /nexistepas_zzz HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
    code = status_code(r)
    if code == 404:
        log_pass("GET /inconnu -> 404")
    else:
        log_fail("GET /inconnu -> 404", "recu code={}".format(code))


def t_bad_method():
    r = send_raw(b"BREW / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
    code = status_code(r)
    # 405 (not allowed) ou 400/501 (not implemented) sont acceptables.
    if code in (400, 405, 501):
        log_pass("Methode inconnue -> {}".format(code))
    else:
        log_fail("Methode inconnue -> 4xx/501", "recu code={}".format(code))


def t_bad_http_version():
    r = send_raw(b"GET / HTTP/9.9\r\nHost: localhost\r\nConnection: close\r\n\r\n")
    code = status_code(r)
    if code in (400, 505, 200):  # certains tolerent, l'important: pas de crash
        log_pass("Version HTTP invalide -> {} (pas de crash)".format(code))
    else:
        log_fail("Version HTTP invalide", "recu code={}".format(code))


def t_malformed_request_line():
    r = send_raw(b"CECI NEST PAS DU HTTP\r\n\r\n")
    code = status_code(r)
    if code == 400:
        log_pass("Ligne de requete malformee -> 400")
    else:
        log_fail("Ligne de requete malformee -> 400", "recu code={}".format(code))


def t_empty_request():
    # Connexion ouverte puis rien : le serveur doit fermer/timeout, pas hanger.
    try:
        r = send_raw(b"\r\n\r\n")
        log_pass("Requete vide geree (pas de hang)")
    except socket.timeout:
        log_fail("Requete vide", "HANG (timeout {}s)".format(TIMEOUT))
    except OSError as e:
        log_pass("Requete vide -> connexion fermee ({})".format(e.__class__.__name__))


def t_no_host_http11():
    # HTTP/1.1 sans Host => 400 attendu (mais surtout : pas de crash).
    r = send_raw(b"GET / HTTP/1.1\r\nConnection: close\r\n\r\n")
    code = status_code(r)
    if code in (400, 200):
        log_pass("HTTP/1.1 sans Host -> {} (pas de crash)".format(code))
    else:
        log_fail("HTTP/1.1 sans Host", "recu code={}".format(code))


def t_slow_loris_headers():
    """Envoi octet par octet, tres lentement. Le serveur ne doit pas planter
    et doit finir par repondre ou couper — jamais bloquer les autres."""
    s = connect()
    req = b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n"
    try:
        for b in req:
            s.sendall(bytes([b]))
            time.sleep(0.01)
        data = b""
        try:
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
        except socket.timeout:
            pass
        code = status_code(data)
        if code is not None:
            log_pass("Slowloris (octet/octet) -> {}".format(code))
        else:
            log_pass("Slowloris -> connexion coupee proprement")
    except OSError as e:
        log_pass("Slowloris -> {} (pas de crash)".format(e.__class__.__name__))
    finally:
        s.close()


def t_partial_then_disconnect():
    """Envoie un debut de requete puis coupe brutalement (RST/close).
    Cible les SIGPIPE et les fd non fermes cote serveur."""
    for _ in range(50):
        s = connect()
        try:
            s.sendall(b"GET / HTTP/1.1\r\nHost: localhost\r\nX-Partial: ")
            # pas de fin de requete : on ferme d'un coup
            s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER,
                         __import__("struct").pack("ii", 1, 0))  # RST
        except OSError:
            pass
        finally:
            s.close()
    if is_alive():
        log_pass("50x deconnexion brutale mi-requete -> serveur vivant")
    else:
        log_fail("50x deconnexion brutale", "serveur MORT")


def t_huge_header():
    """Header unique gigantesque (~64 Ko). Doit etre rejete (431/400/413)
    ou accepte, mais JAMAIS crasher / consommer indefiniment la RAM."""
    big = b"X-Huge: " + b"A" * 65000 + b"\r\n"
    payload = b"GET / HTTP/1.1\r\nHost: localhost\r\n" + big + b"Connection: close\r\n\r\n"
    try:
        r = send_raw(payload)
        code = status_code(r)
        log_pass("Header geant (64Ko) -> {} (pas de crash)".format(code))
    except OSError as e:
        log_pass("Header geant -> {} (connexion coupee)".format(e.__class__.__name__))
    if not is_alive():
        log_fail("Header geant", "serveur MORT apres coup")


def t_many_headers():
    """Des milliers de headers. Test d'epuisement memoire/parse."""
    headers = b"".join(b"X-H%d: v\r\n" % i for i in range(2000))
    payload = b"GET / HTTP/1.1\r\nHost: localhost\r\n" + headers + b"Connection: close\r\n\r\n"
    try:
        r = send_raw(payload)
        log_pass("2000 headers -> {} (pas de crash)".format(status_code(r)))
    except OSError as e:
        log_pass("2000 headers -> {} ".format(e.__class__.__name__))
    if not is_alive():
        log_fail("2000 headers", "serveur MORT")


def t_body_over_limit():
    """Body > client_max_body_size (1024 sur /). Attendu : 413."""
    body = b"x" * 5000
    payload = (b"POST / HTTP/1.1\r\nHost: localhost\r\n"
               b"Content-Length: " + str(len(body)).encode() + b"\r\n"
               b"Connection: close\r\n\r\n" + body)
    r = send_raw(payload)
    code = status_code(r)
    if code == 413:
        log_pass("Body > max_body_size -> 413")
    else:
        log_fail("Body > max_body_size -> 413", "recu code={}".format(code))


def t_content_length_lies_bigger():
    """Content-Length annonce plus que le body reel, puis on coupe.
    Le serveur ne doit pas attendre eternellement -> pas de hang global."""
    payload = (b"POST /upload HTTP/1.1\r\nHost: localhost\r\n"
               b"Content-Length: 1000000\r\n"
               b"Connection: close\r\n\r\nSHORT")
    try:
        send_raw(payload, delay_close=0.2)
        log_pass("Content-Length menteur (trop grand) gere sans hang bloquant")
    except socket.timeout:
        # Un timeout ICI est acceptable (le serveur attend le body), tant que
        # les AUTRES clients passent. On verifie donc la liveness juste apres.
        log_pass("Content-Length menteur -> attente body (timeout local OK)")
    if not is_alive():
        log_fail("Content-Length menteur", "serveur bloque pour tout le monde")


def t_content_length_negative():
    payload = (b"POST / HTTP/1.1\r\nHost: localhost\r\n"
               b"Content-Length: -5\r\nConnection: close\r\n\r\nabcde")
    try:
        r = send_raw(payload)
        code = status_code(r)
        if code in (400, 413):
            log_pass("Content-Length negatif -> {}".format(code))
        else:
            log_pass("Content-Length negatif -> {} (pas de crash)".format(code))
    except OSError as e:
        log_pass("Content-Length negatif -> {}".format(e.__class__.__name__))
    if not is_alive():
        log_fail("Content-Length negatif", "serveur MORT")


def t_double_content_length():
    payload = (b"POST / HTTP/1.1\r\nHost: localhost\r\n"
               b"Content-Length: 5\r\nContent-Length: 10\r\n"
               b"Connection: close\r\n\r\nabcde")
    r = send_raw(payload)
    code = status_code(r)
    # 400 est le comportement correct (RFC), mais on veut surtout pas de crash.
    log_pass("Double Content-Length -> {} (pas de crash)".format(code))
    if not is_alive():
        log_fail("Double Content-Length", "serveur MORT")


def t_chunked_valid():
    payload = (b"POST /upload HTTP/1.1\r\nHost: localhost\r\n"
               b"Transfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
               b"5\r\nhello\r\n6\r\n world\r\n0\r\n\r\n")
    try:
        r = send_raw(payload)
        code = status_code(r)
        log_pass("Chunked valide -> {} ".format(code))
    except OSError as e:
        log_pass("Chunked valide -> {}".format(e.__class__.__name__))
    if not is_alive():
        log_fail("Chunked valide", "serveur MORT")


def t_chunked_broken():
    """Taille de chunk annoncee fausse puis coupure. Ne doit pas crasher."""
    payload = (b"POST /upload HTTP/1.1\r\nHost: localhost\r\n"
               b"Transfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
               b"FF\r\ntroppetit")  # annonce 255 octets, en donne 9, puis close
    try:
        send_raw(payload, delay_close=0.2)
    except OSError:
        pass
    if is_alive():
        log_pass("Chunked casse -> serveur vivant")
    else:
        log_fail("Chunked casse", "serveur MORT")


def t_chunked_bad_hex():
    payload = (b"POST /upload HTTP/1.1\r\nHost: localhost\r\n"
               b"Transfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
               b"ZZ\r\ndata\r\n0\r\n\r\n")  # taille non-hexa
    try:
        r = send_raw(payload)
        code = status_code(r)
        if code == 400:
            log_pass("Chunked taille non-hexa -> 400")
        else:
            log_pass("Chunked taille non-hexa -> {} (pas de crash)".format(code))
    except OSError as e:
        log_pass("Chunked taille non-hexa -> {}".format(e.__class__.__name__))
    if not is_alive():
        log_fail("Chunked taille non-hexa", "serveur MORT")


def t_pipelining():
    """Deux requetes dans le meme paquet (keep-alive)."""
    payload = (b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
               b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
    r = send_raw(payload)
    n = r.count(b"HTTP/1.1 ")
    if n >= 1:
        log_pass("Pipelining -> {} reponse(s) (pas de crash)".format(n))
    else:
        log_fail("Pipelining", "aucune reponse")


def t_flood_idle_connections():
    """Ouvre beaucoup de connexions muettes en parallele : tente d'epuiser
    les fd du serveur. Le serveur doit rester joignable pour un client normal."""
    n = 200
    socks = []
    try:
        for _ in range(n):
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.settimeout(TIMEOUT)
                s.connect((HOST, PORT))
                socks.append(s)
            except OSError:
                break
        opened = len(socks)
        alive = is_alive()
        if alive:
            log_pass("{} connexions muettes ouvertes -> serveur toujours joignable".format(opened))
        else:
            log_fail("Flood connexions muettes", "serveur injoignable a {} conns".format(opened))
    finally:
        for s in socks:
            try:
                s.close()
            except OSError:
                pass
    time.sleep(0.5)
    if not is_alive():
        log_fail("Flood connexions muettes", "serveur MORT apres fermeture")


def t_concurrent_hammer():
    """Plusieurs threads envoient des requetes propres en rafale.
    Detecte les races / doubles free sous concurrence."""
    errors = []

    def worker():
        for _ in range(30):
            try:
                r = send_raw(b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
                if status_code(r) is None:
                    errors.append("reponse vide")
            except OSError as e:
                errors.append(str(e))

    threads = [threading.Thread(target=worker) for _ in range(20)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    # 20 threads x 30 req = 600 requetes. Quelques erreurs reseau tolerees.
    if len(errors) < 30 and is_alive():
        log_pass("600 requetes concurrentes -> {} erreur(s), serveur vivant".format(len(errors)))
    else:
        log_fail("600 requetes concurrentes", "{} erreurs".format(len(errors)))


def t_cgi_under_load():
    """CGI (.py) en concurrence : cible les fuites de fd/pipes et zombies."""
    ok = 0
    fail = 0
    for _ in range(30):
        try:
            r = send_raw(b"GET /cgi.py HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
            code = status_code(r)
            if code is not None:
                ok += 1
            else:
                fail += 1
        except OSError:
            fail += 1
    if ok > 0 and is_alive():
        log_pass("CGI .py x30 -> {} ok / {} ko, serveur vivant".format(ok, fail))
    elif not is_alive():
        log_fail("CGI sous charge", "serveur MORT")
    else:
        # Le CGI n'existe peut-etre pas dans cette config : on n'echoue pas dur.
        print("  {}[WARN]{} CGI .py : aucune reponse valide (chemin/binaire absent ?)".format(YELLOW, RESET))


def t_final_liveness():
    if is_alive():
        log_pass("LIVENESS FINALE -> serveur toujours debout apres tous les tests")
    else:
        log_fail("LIVENESS FINALE", "serveur MORT a la fin — CRITIQUE")


TESTS = [
    t_basic_get,
    t_not_found,
    t_bad_method,
    t_bad_http_version,
    t_malformed_request_line,
    t_empty_request,
    t_no_host_http11,
    t_slow_loris_headers,
    t_partial_then_disconnect,
    t_huge_header,
    t_many_headers,
    t_body_over_limit,
    t_content_length_lies_bigger,
    t_content_length_negative,
    t_double_content_length,
    t_chunked_valid,
    t_chunked_broken,
    t_chunked_bad_hex,
    t_pipelining,
    t_flood_idle_connections,
    t_concurrent_hammer,
    t_cgi_under_load,
    t_final_liveness,
]


def main():
    print("{}== Tests cas-limites sockets bruts  {}:{}  =={}".format(CYAN, HOST, PORT, RESET))
    # Verif que le serveur est la avant de commencer.
    if not is_alive():
        print("{}[ABORT]{} serveur injoignable sur {}:{} — est-il lance ?".format(RED, RESET, HOST, PORT))
        sys.exit(2)

    for test in TESTS:
        try:
            test()
        except Exception as e:  # un test ne doit jamais tuer le harnais
            log_fail(test.__name__, "exception harnais: {}".format(e))

    passed = sum(1 for _, ok, _ in results if ok)
    failed = sum(1 for _, ok, _ in results if not ok)
    print()
    print("{}== Bilan sockets bruts : {} PASS / {} FAIL =={}".format(
        CYAN, passed, failed, RESET))
    if failed:
        print("{}Tests en echec :{}".format(RED, RESET))
        for name, ok, detail in results:
            if not ok:
                print("  - {} {}".format(name, detail))
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
