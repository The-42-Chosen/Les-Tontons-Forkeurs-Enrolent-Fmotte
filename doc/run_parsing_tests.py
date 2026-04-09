#!/usr/bin/env python3

import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BIN_PATH = ROOT / "Webserv"

TESTS = [
    ("valid_default_doc_config", "doc/config_file", True, "Config de reference du projet."),
    ("valid_minimal", "doc/parsing_tests/valid_minimal.conf", True, "Serveur minimal valide."),
    ("valid_two_servers", "doc/parsing_tests/valid_two_servers.conf", True, "Deux blocs server valides."),
    ("invalid_unknown_directive_server", "doc/parsing_tests/invalid_unknown_directive_server.conf", False, "Directive inconnue dans server."),
    ("invalid_unknown_directive_location", "doc/parsing_tests/invalid_unknown_directive_location.conf", False, "Directive inconnue dans location."),
    ("invalid_missing_semicolon_root", "doc/parsing_tests/invalid_missing_semicolon_root.conf", False, "Point-virgule manquant apres root."),
    ("invalid_missing_closing_brace_server", "doc/parsing_tests/invalid_missing_closing_brace_server.conf", False, "Accolade fermante manquante."),
    ("invalid_bad_autoindex_server", "doc/parsing_tests/invalid_bad_autoindex_server.conf", False, "Valeur autoindex invalide au niveau server."),
    ("invalid_bad_autoindex_location", "doc/parsing_tests/invalid_bad_autoindex_location.conf", False, "Valeur autoindex invalide dans location."),
    ("invalid_non_numeric_port", "doc/parsing_tests/invalid_non_numeric_port.conf", False, "Port non numerique."),
    ("invalid_non_numeric_client_size", "doc/parsing_tests/invalid_non_numeric_client_size.conf", False, "client_max_body_size non numerique."),
    ("invalid_error_page_missing_value", "doc/parsing_tests/invalid_error_page_missing_value.conf", False, "error_page sans chemin."),
    ("invalid_return_missing_value", "doc/parsing_tests/invalid_return_missing_value.conf", False, "return sans URL/message."),
    ("invalid_listen_extra_colon", "doc/parsing_tests/invalid_listen_extra_colon.conf", False, "listen avec format ip:port:port."),
    ("invalid_server_name_no_value", "doc/parsing_tests/invalid_server_name_no_value.conf", False, "server_name sans valeur."),
    ("invalid_location_no_path", "doc/parsing_tests/invalid_location_no_path.conf", False, "location sans chemin."),
    ("invalid_allowed_methods_unknown", "doc/parsing_tests/invalid_allowed_methods_unknown.conf", False, "allowed_methods avec methode inconnue."),
    ("invalid_unclosed_quote_return", "doc/parsing_tests/invalid_unclosed_quote_return.conf", False, "Guillemet non ferme dans return."),
    ("invalid_empty", "doc/parsing_tests/invalid_empty.conf", False, "Fichier vide."),
    ("invalid_comment_eof", "doc/parsing_tests/invalid_comment_eof.conf", True, "Commentaire final sans newline (doit rester valide)."),
]


ROOT_CAUSES = {
    "invalid_unknown_directive_server": "Boucle server sans fallback: aucun parseur ne consomme le token inconnu, la boucle while reste bloquee.",
    "invalid_unknown_directive_location": "Boucle location sans fallback: token inconnu jamais consomme, boucle infinie.",
    "invalid_missing_semicolon_root": "Le parseur efface des tokens en supposant la presence de ;, ce qui desynchronise le flux et casse la suite.",
    "invalid_missing_closing_brace_server": "Acces a tokens[0] sans verifier la fin du vecteur: comportement indefini (souvent crash/segfault).",
    "invalid_bad_autoindex_server": "La fonction signale une erreur mais le code de retour est ignore dans la boucle principale.",
    "invalid_bad_autoindex_location": "Meme probleme: erreur detectee mais non propagee, parsing continue et retourne 0.",
    "invalid_non_numeric_port": "Erreur detectee localement mais non propagee jusqu au main, le programme finit en succes.",
    "invalid_non_numeric_client_size": "Erreur de conversion detectee mais non propagee en code de sortie.",
    "invalid_error_page_missing_value": "Aucune verification d arite (nombre d arguments): le ; est pris comme valeur valide.",
    "invalid_return_missing_value": "Aucune verification d arite: ; devient valeur de return au lieu de lever une erreur.",
    "invalid_listen_extra_colon": "Format listen trop permissif: dernier segment numerique ecrase port precedent sans rejet.",
    "invalid_server_name_no_value": "Aucune contrainte sur server_name vide, le parser accepte server_name ;.",
    "invalid_location_no_path": "Syntaxe location non valide acceptee: { est pris comme nom de location.",
    "invalid_allowed_methods_unknown": "Branche else vide dans allowed_methods: methode inconnue ignoree sans erreur.",
    "invalid_unclosed_quote_return": "Gestion des guillemets incomplete (fusion 2 tokens max), pas de validation de fermeture de quote.",
    "invalid_empty": "Aucun garde-fou sur tokens vide avant acces tokens[0], provoque comportement indefini/crash.",
}


def run_one(test_name, cfg, should_pass):
    cmd = [str(BIN_PATH), cfg]
    try:
        proc = subprocess.run(
            cmd,
            cwd=str(ROOT),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            timeout=2,
        )
        status = proc.returncode
        timed_out = False
        output = ((proc.stdout or "") + "\n" + (proc.stderr or "")).strip()
    except subprocess.TimeoutExpired as exp:
        status = None
        timed_out = True
        out = exp.stdout or ""
        err = exp.stderr or ""
        if isinstance(out, (bytes, bytearray)):
            out = out.decode(errors="replace")
        if isinstance(err, (bytes, bytearray)):
            err = err.decode(errors="replace")
        output = (out + "\n" + err).strip()

    if timed_out:
        verdict = "FAIL"
        reason = "timeout (boucle infinie probable)"
    elif should_pass and status != 0:
        verdict = "FAIL"
        reason = f"attendu exit 0, obtenu {status}"
    elif (not should_pass) and status == 0:
        verdict = "FAIL"
        reason = "config invalide acceptee (exit 0)"
    else:
        verdict = "PASS"
        reason = "ok"

    return {
        "name": test_name,
        "cfg": cfg,
        "should_pass": should_pass,
        "status": status,
        "timed_out": timed_out,
        "verdict": verdict,
        "reason": reason,
        "output": output,
    }


def write_failed_report(results):
    failed = [r for r in results if r["verdict"] == "FAIL"]
    report_path = ROOT / "doc" / "failed_tests"

    with report_path.open("w", encoding="utf-8") as rep:
        rep.write("Webserv - Rapport des tests de parsing qui echouent\n")
        rep.write("Date: 2026-04-09\n")
        rep.write("Binaire teste: ./Webserv\n\n")
        rep.write(f"Total tests executes: {len(results)}\n")
        rep.write(f"Nombre de tests en echec: {len(failed)}\n\n")

        if not failed:
            rep.write("Aucun test en echec sur cette campagne.\n")
            return report_path, 0

        for idx, result in enumerate(failed, start=1):
            observed = "TIMEOUT" if result["timed_out"] else f"exit {result['status']}"
            expected = "exit 0" if result["should_pass"] else "exit non-zero"
            rep.write(f"{idx}. {result['name']}\n")
            rep.write(f"   fichier: {result['cfg']}\n")
            rep.write(f"   attendu: {expected}\n")
            rep.write(f"   observe: {observed}\n")
            rep.write(f"   pourquoi ca foire: {result['reason']}\n")
            comment = ROOT_CAUSES.get(result["name"], "Cause non determinee automatiquement.")
            rep.write(f"   commentaire technique: {comment}\n")

            if result["output"]:
                snippet = result["output"].replace("\r", "")
                if len(snippet) > 600:
                    snippet = snippet[:600] + "..."
                rep.write("   extrait sortie:\n")
                for line in snippet.splitlines()[:10]:
                    rep.write(f"      {line}\n")

            rep.write("\n")

    return report_path, len(failed)


def main():
    if not BIN_PATH.exists():
        print("Erreur: binaire Webserv absent. Lance make avant ce script.")
        raise SystemExit(1)

    results = [run_one(name, cfg, should_pass) for name, cfg, should_pass, _note in TESTS]

    print("=== RESULTATS ===")
    for res in results:
        got = "TIMEOUT" if res["timed_out"] else str(res["status"])
        expected = "valid->0" if res["should_pass"] else "invalid->nonzero"
        print(f"[{res['verdict']}] {res['name']}: expected={expected}, got={got}, reason={res['reason']}")

    report_path, failed_count = write_failed_report(results)
    print(f"\nRapport ecrit dans: {report_path}")
    print(f"Failed count: {failed_count}")


if __name__ == "__main__":
    main()
