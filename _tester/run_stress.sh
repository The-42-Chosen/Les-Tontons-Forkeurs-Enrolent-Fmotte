#!/usr/bin/env bash
# ============================================================================
#  run_stress.sh — Harnais de stress "fiable" pour webserv (42), cible UBUNTU.
#
#  Fait, dans l'ordre :
#    0. build (make re) depuis la racine du repo
#    1. PASSE VALGRIND (charge legere) :
#         - lance webserv sous valgrind avec leak-check + --track-fds=yes
#         - joue les cas-limites (stress_test.py) + un petit siege
#         - arret PROPRE via SIGINT pour que valgrind rende son verdict
#         - parse le log : leaks, fd ouverts au exit, erreurs memoire
#    2. PASSE NATIVE (charge lourde) :
#         - lance webserv sans valgrind (vitesse reelle)
#         - siege -b (dispo >= 99.5% attendu par l'eval)
#         - echantillonne RSS + nb de fd + connexions CLOSE_WAIT pendant siege
#         - rejoue les cas-limites sous charge
#    3. VERDICT global + code retour (0 = OK pour viser 125)
#
#  Usage :
#    ./run_stress.sh                 # tout (valgrind leger + native lourd)
#    ./run_stress.sh --valgrind-only # seulement la passe valgrind
#    ./run_stress.sh --native-only   # seulement la passe charge lourde
#    ./run_stress.sh --quick         # versions courtes (dev iteratif)
#
#  Variables surchargées possibles :
#    PORT=8080 HOST=127.0.0.1 CONF=... SIEGE_TIME=1M SIEGE_CONC=25 ./run_stress.sh
# ============================================================================

set -u

# ------------------------------------------------------------------ chemins
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BIN="$REPO_ROOT/Webserv"
CONF="${CONF:-$SCRIPT_DIR/config/webserv_test.conf}"
PY_TESTS="$SCRIPT_DIR/stress_test.py"
LOG_DIR="$SCRIPT_DIR/logs"
VALGRIND_LOG="$LOG_DIR/valgrind.log"
SERVER_LOG="$LOG_DIR/server.log"

# ------------------------------------------------------------------ reglages
HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-8080}"
SIEGE_TIME="${SIEGE_TIME:-1M}"      # duree siege passe native
SIEGE_CONC="${SIEGE_CONC:-25}"      # clients concurrents
SIEGE_TIME_VG="${SIEGE_TIME_VG:-15S}"   # siege court sous valgrind
SIEGE_CONC_VG="${SIEGE_CONC_VG:-5}"

MODE="all"
for arg in "$@"; do
    case "$arg" in
        --valgrind-only) MODE="valgrind" ;;
        --native-only)   MODE="native" ;;
        --quick)         SIEGE_TIME="20S"; SIEGE_TIME_VG="8S" ;;
        -h|--help)       grep '^#' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "arg inconnu: $arg" ;;
    esac
done

# ------------------------------------------------------------------ couleurs
G="\033[32m"; R="\033[31m"; Y="\033[33m"; C="\033[36m"; B="\033[1m"; N="\033[0m"

FAILURES=0
WARNINGS=0
fail()  { echo -e "${R}${B}[FAIL]${N} $*"; FAILURES=$((FAILURES+1)); }
warn()  { echo -e "${Y}[WARN]${N} $*"; WARNINGS=$((WARNINGS+1)); }
ok()    { echo -e "${G}[ OK ]${N} $*"; }
info()  { echo -e "${C}==>${N} $*"; }
title() { echo -e "\n${B}${C}######## $* ########${N}"; }

SERVER_PID=""

cleanup() {
    if [ -n "$SERVER_PID" ] && kill -0 "$SERVER_PID" 2>/dev/null; then
        kill -INT "$SERVER_PID" 2>/dev/null
        sleep 1
        kill -KILL "$SERVER_PID" 2>/dev/null
    fi
    # Filet de securite : tue tout webserv residuel lance par ce harnais.
    pkill -INT -f "valgrind.*Webserv" 2>/dev/null
}
trap cleanup EXIT INT TERM

# ---------------------------------------------------------------- pre-vols
check_env() {
    title "Environnement"
    [ "$(uname)" = "Linux" ] || warn "Pas sous Linux : valgrind/proc peuvent manquer. (cible = Ubuntu)"
    command -v make    >/dev/null || { fail "make introuvable"; exit 1; }
    command -v python3 >/dev/null || { fail "python3 introuvable"; exit 1; }
    command -v siege   >/dev/null && ok "siege dispo" || warn "siege absent (sudo apt install siege) — passe native limitee"
    if [ "$MODE" != "native" ]; then
        command -v valgrind >/dev/null && ok "valgrind dispo" || { warn "valgrind absent — passe valgrind sautee"; [ "$MODE" = "valgrind" ] && exit 1; }
    fi
    [ -f "$CONF" ] || { fail "config absente: $CONF"; exit 1; }
    mkdir -p "$LOG_DIR"
    ok "config: $CONF   port: $PORT"
}

build() {
    title "Build (make re)"
    if make -C "$REPO_ROOT" re >/dev/null 2>&1 && [ -x "$BIN" ]; then
        ok "compilation reussie -> $BIN"
    else
        fail "compilation echouee (make -C $REPO_ROOT re)"
        exit 1
    fi
}

# Attend que le port ecoute (max ~10s). $1 = pid a surveiller.
wait_for_port() {
    local pid="$1" i
    for i in $(seq 1 100); do
        kill -0 "$pid" 2>/dev/null || { fail "le serveur est mort au demarrage (voir $SERVER_LOG)"; return 1; }
        if (exec 3<>"/dev/tcp/$HOST/$PORT") 2>/dev/null; then
            exec 3>&- 3<&- 2>/dev/null
            return 0
        fi
        sleep 0.1
    done
    fail "le port $PORT n'ecoute pas apres 10s"
    return 1
}

count_fds()   { ls "/proc/$1/fd" 2>/dev/null | wc -l; }
get_rss_kb()  { awk '/VmRSS/{print $2}' "/proc/$1/status" 2>/dev/null; }

# =========================================================================
#  PASSE 1 : VALGRIND
# =========================================================================
run_valgrind_pass() {
    command -v valgrind >/dev/null || return 0
    title "PASSE VALGRIND (fuites memoire + descripteurs)"
    : > "$VALGRIND_LOG"

    # Flags clefs :
    #   --leak-check=full --show-leak-kinds=all : toute fuite, meme "still reachable"
    #   --errors-for-leak-kinds=all             : les fuites comptent comme erreurs
    #   --track-fds=yes                         : liste les fd encore ouverts a la sortie
    #   --error-exitcode=42                     : code sortie != 0 si erreur (fiable)
    #   --trace-children=no : on NE suit PAS les CGI (execve python/php) — hors sujet
    #   --child-silent-after-fork=yes : pas de bruit valgrind cote fork CGI
    ( cd "$REPO_ROOT" && exec valgrind \
        --leak-check=full \
        --show-leak-kinds=all \
        --errors-for-leak-kinds=all \
        --track-fds=yes \
        --trace-children=no \
        --child-silent-after-fork=yes \
        --error-exitcode=42 \
        --log-file="$VALGRIND_LOG" \
        "$BIN" "$CONF" >"$SERVER_LOG" 2>&1 ) &
    SERVER_PID=$!

    if ! wait_for_port "$SERVER_PID"; then
        cat "$SERVER_LOG"
        return
    fi
    ok "serveur lance sous valgrind (pid $SERVER_PID)"

    info "cas-limites (sockets bruts) sous valgrind..."
    python3 "$PY_TESTS" "$HOST" "$PORT" || fail "des cas-limites ont echoue (passe valgrind)"

    if command -v siege >/dev/null; then
        info "petit siege sous valgrind (${SIEGE_CONC_VG}c / ${SIEGE_TIME_VG})..."
        siege -b -c "$SIEGE_CONC_VG" -t "$SIEGE_TIME_VG" "http://$HOST:$PORT/" >/dev/null 2>&1
    fi

    # Arret PROPRE : indispensable pour que valgrind produise son rapport.
    info "arret propre (SIGINT) pour le rapport valgrind..."
    kill -INT "$SERVER_PID" 2>/dev/null
    for i in $(seq 1 50); do kill -0 "$SERVER_PID" 2>/dev/null || break; sleep 0.2; done
    if kill -0 "$SERVER_PID" 2>/dev/null; then
        warn "le serveur n'a pas quitte sur SIGINT en 10s — kill force (rapport valgrind incomplet)"
        kill -KILL "$SERVER_PID" 2>/dev/null
    fi
    wait "$SERVER_PID" 2>/dev/null
    SERVER_PID=""

    analyze_valgrind
}

analyze_valgrind() {
    title "Analyse valgrind"
    [ -f "$VALGRIND_LOG" ] || { fail "pas de log valgrind"; return; }

    # --- Fuites memoire ---
    local def_lost indir_lost poss_lost reachable
    def_lost=$(grep -oP 'definitely lost: \K[0-9,]+' "$VALGRIND_LOG" | tr -d ',' | tail -1)
    indir_lost=$(grep -oP 'indirectly lost: \K[0-9,]+' "$VALGRIND_LOG" | tr -d ',' | tail -1)
    poss_lost=$(grep -oP 'possibly lost: \K[0-9,]+'  "$VALGRIND_LOG" | tr -d ',' | tail -1)
    reachable=$(grep -oP 'still reachable: \K[0-9,]+' "$VALGRIND_LOG" | tr -d ',' | tail -1)

    [ "${def_lost:-0}" -gt 0 2>/dev/null ]   && fail "definitely lost: $def_lost octets"   || ok "definitely lost: 0"
    [ "${indir_lost:-0}" -gt 0 2>/dev/null ] && fail "indirectly lost: $indir_lost octets" || ok "indirectly lost: 0"
    [ "${poss_lost:-0}" -gt 0 2>/dev/null ]  && warn "possibly lost: $poss_lost octets (souvent faux positif STL)" || ok "possibly lost: 0"
    [ "${reachable:-0}" -gt 0 2>/dev/null ]  && warn "still reachable: $reachable octets (globals/singletons — a verifier)" || ok "still reachable: 0"

    # --- Erreurs memoire (invalid read/write, uninit, etc.) ---
    local err_sum
    err_sum=$(grep -oP 'ERROR SUMMARY: \K[0-9]+' "$VALGRIND_LOG" | tail -1)
    if [ "${err_sum:-0}" -gt 0 2>/dev/null ]; then
        fail "ERROR SUMMARY: $err_sum erreur(s) memoire — voir $VALGRIND_LOG"
        grep -E 'Invalid (read|write)|uninitialised|Mismatched|double free' "$VALGRIND_LOG" | sort -u | head -5 | sed 's/^/       /'
    else
        ok "ERROR SUMMARY: 0 erreur memoire"
    fi

    # --- Descripteurs de fichiers (--track-fds=yes) ---
    # A l'exit, valgrind liste les fd ouverts. 0,1,2 (stdio) sont normaux/attendus.
    if grep -q 'FILE DESCRIPTORS' "$VALGRIND_LOG"; then
        local total_open
        total_open=$(grep -oP 'FILE DESCRIPTORS: \K[0-9]+' "$VALGRIND_LOG" | tail -1)
        # 3 = les 3 std (inherited). Plus que 3 => fd fuite (socket/pipe CGI non ferme).
        if [ "${total_open:-3}" -gt 3 2>/dev/null ]; then
            fail "FD ouverts a la sortie: $total_open (>3) — descripteurs non fermes"
            grep -A1 'Open file descriptor' "$VALGRIND_LOG" | grep -v inherited | head -8 | sed 's/^/       /'
        else
            ok "FD ouverts a la sortie: $total_open (uniquement stdin/stdout/stderr)"
        fi
    else
        warn "pas de section FILE DESCRIPTORS (arret non propre ? --track-fds actif ?)"
    fi

    echo -e "   ${C}(log complet: $VALGRIND_LOG)${N}"
}

# =========================================================================
#  PASSE 2 : NATIVE (charge lourde + monitoring)
# =========================================================================
run_native_pass() {
    title "PASSE NATIVE (charge lourde, dispo/RSS/FD)"

    ( cd "$REPO_ROOT" && exec "$BIN" "$CONF" >"$SERVER_LOG" 2>&1 ) &
    SERVER_PID=$!
    if ! wait_for_port "$SERVER_PID"; then
        cat "$SERVER_LOG"; return
    fi
    ok "serveur lance en natif (pid $SERVER_PID)"

    local rss0 fd0
    rss0=$(get_rss_kb "$SERVER_PID"); fd0=$(count_fds "$SERVER_PID")
    info "etat initial : RSS=${rss0:-?} Ko, FD=${fd0:-?}"

    # ---- cas-limites d'abord (a froid) ----
    info "cas-limites (sockets bruts)..."
    python3 "$PY_TESTS" "$HOST" "$PORT" || fail "des cas-limites ont echoue (passe native)"

    # ---- siege + monitoring en parallele ----
    if command -v siege >/dev/null; then
        info "siege -b -c $SIEGE_CONC -t $SIEGE_TIME sur http://$HOST:$PORT/ ..."
        local siege_out="$LOG_DIR/siege.log"
        siege -b -c "$SIEGE_CONC" -t "$SIEGE_TIME" "http://$HOST:$PORT/" >"$siege_out" 2>&1 &
        local siege_pid=$!

        # Echantillonnage pendant que siege tourne.
        local rss_max="${rss0:-0}" fd_max="${fd0:-0}" samples=0
        while kill -0 "$siege_pid" 2>/dev/null; do
            local rss fd
            rss=$(get_rss_kb "$SERVER_PID"); fd=$(count_fds "$SERVER_PID")
            [ -n "$rss" ] && [ "$rss" -gt "$rss_max" ] 2>/dev/null && rss_max="$rss"
            [ -n "$fd" ] && [ "$fd" -gt "$fd_max" ] 2>/dev/null && fd_max="$fd"
            samples=$((samples+1))
            sleep 1
        done
        wait "$siege_pid" 2>/dev/null

        # --- Dispo siege ---
        # siege ecrit "Availability: 100.00 %" (ou en localise). On parse le nombre.
        local avail
        avail=$(grep -iE 'availab' "$siege_out" | grep -oP '[0-9]+\.[0-9]+' | head -1)
        local trans fail_tx
        trans=$(grep -iE 'transactions:' "$siege_out" | grep -oP '[0-9,]+' | head -1 | tr -d ',')
        fail_tx=$(grep -iE 'failed transactions' "$siege_out" | grep -oP '[0-9,]+' | head -1 | tr -d ',')
        echo
        info "resultats siege : dispo=${avail:-?}%  transactions=${trans:-?}  echecs=${fail_tx:-?}"
        if [ -n "$avail" ]; then
            # comparaison flottante via awk : seuil eval = 99.5%
            if awk "BEGIN{exit !($avail >= 99.5)}"; then
                ok "disponibilite ${avail}% >= 99.5% (seuil eval)"
            else
                fail "disponibilite ${avail}% < 99.5% (seuil eval NON atteint)"
            fi
        else
            warn "dispo siege non lue (voir $siege_out)"
        fi

        # --- Bilan RSS / FD ---
        local rss_now fd_now
        rss_now=$(get_rss_kb "$SERVER_PID"); fd_now=$(count_fds "$SERVER_PID")
        info "RSS: init=${rss0:-?} Ko  pic=${rss_max} Ko  final=${rss_now:-?} Ko   (sur $samples echantillons)"
        info "FD : init=${fd0:-?}  pic=${fd_max}  final=${fd_now:-?}"

        # Croissance memoire : le RSS final ne doit pas exploser vs initial.
        if [ -n "$rss_now" ] && [ -n "$rss0" ] && [ "$rss0" -gt 0 ] 2>/dev/null; then
            local growth=$(( rss_now * 100 / rss0 ))
            if [ "$growth" -gt 150 ]; then
                fail "RSS final = ${growth}% de l'initial (>150%) — fuite memoire probable sous charge"
            else
                ok "RSS final = ${growth}% de l'initial (stable)"
            fi
        fi
        # FD : doivent redescendre au niveau de base apres la charge.
        if [ -n "$fd_now" ] && [ -n "$fd0" ]; then
            if [ "$fd_now" -gt $(( fd0 + 5 )) ]; then
                fail "FD final=$fd_now >> init=$fd0 — descripteurs (sockets/pipes CGI) non fermes"
            else
                ok "FD revenus a la normale (init=$fd0, final=$fd_now)"
            fi
        fi
    else
        warn "siege absent : test de disponibilite saute (installe-le pour l'eval !)"
    fi

    # ---- connexions pendantes (CLOSE_WAIT = fd cote serveur pas fermes) ----
    if command -v ss >/dev/null; then
        local closewait
        closewait=$(ss -tan 2>/dev/null | awk -v p=":$PORT" '$0 ~ p && /CLOSE-WAIT/' | wc -l)
        if [ "$closewait" -gt 5 ]; then
            fail "$closewait connexions CLOSE-WAIT sur :$PORT — le serveur ne ferme pas ses sockets"
        else
            ok "connexions CLOSE-WAIT: $closewait (ok)"
        fi
    fi

    # ---- rejeu cas-limites sous charge residuelle + liveness ----
    info "rejeu cas-limites apres la charge..."
    python3 "$PY_TESTS" "$HOST" "$PORT" >/dev/null 2>&1 \
        && ok "cas-limites re-passent apres charge" \
        || fail "des cas-limites echouent APRES la charge (degradation)"

    # ---- endurance courte : le serveur doit tenir sans etre relance ----
    if kill -0 "$SERVER_PID" 2>/dev/null; then
        ok "serveur toujours vivant apres toute la passe native (pas de relance)"
    else
        fail "serveur MORT pendant la passe native — voir $SERVER_LOG"
    fi

    kill -INT "$SERVER_PID" 2>/dev/null
    sleep 1
    kill -KILL "$SERVER_PID" 2>/dev/null
    SERVER_PID=""
}

# =========================================================================
#  MAIN
# =========================================================================
echo -e "${B}${C}"
echo "  ================================================"
echo "   WEBSERV — HARNAIS DE STRESS (cible Ubuntu/42)"
echo "  ================================================"
echo -e "${N}"

check_env
build

case "$MODE" in
    valgrind) run_valgrind_pass ;;
    native)   run_native_pass ;;
    all)      run_valgrind_pass; run_native_pass ;;
esac

# ------------------------------------------------------------------ verdict
title "VERDICT GLOBAL"
if [ "$FAILURES" -eq 0 ]; then
    echo -e "${G}${B}  TOUT EST VERT — 0 echec, $WARNINGS avertissement(s).${N}"
    echo -e "${G}  Aucune fuite memoire/FD detectee, dispo OK, serveur stable.${N}"
    echo -e "${G}  Tu peux viser les 125. Relis quand meme les WARN ci-dessus.${N}"
    exit 0
else
    echo -e "${R}${B}  $FAILURES ECHEC(S) — a corriger avant l'eval.${N}  ($WARNINGS warn)"
    echo -e "${R}  Logs : $LOG_DIR (valgrind.log, server.log, siege.log)${N}"
    exit 1
fi
