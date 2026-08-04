import os
import signal
import socket
import subprocess
import sys
import time
import unittest

VALGRIND_LOG = "valgrind.log"

def wait_for_server(host="127.0.0.1", port=8002, timeout=15):
    start = time.time()
    while time.time() - start < timeout:
        try:
            with socket.create_connection((host, port), timeout=1):
                return
        except OSError:
            time.sleep(0.1)
    raise RuntimeError("Server didn't start in time.")

def analyse_valgrind(logfile):
    print("\n====================")
    print("VALGRIND REPORT")
    print("====================")
    try:
        with open(logfile, "r") as f:
            content = f.read()
        if "LEAK SUMMARY:" not in content and "ERROR SUMMARY:" not in content:
            print("⚠️ Incomplete Valgrind report (process terminated before analysis finished).")
            return
        if "All heap blocks were freed -- no leaks are possible" in content:
            print("✅ NO MEMORY LEAK")
        else:
            print("⚠️ LEAKS FOUND")

            for line in content.splitlines():
                if "ERROR SUMMARY" in line or "lost:" in line:
                    print(line)
    except FileNotFoundError:
        print("Valgrind log not found.")


class EmojiTestResult(unittest.TextTestResult):

    def startTest(self, test):
        description = test.shortDescription()
        if not description:
            description = test.id().split('.')[-1]
        print(f"🔍 {description:<45}", end="", flush=True)

    def addSuccess(self, test):
        super().addSuccess(test)
        print(" 🐱 PASSED")

    def addFailure(self, test, err):
        super().addFailure(test, err)
        print(" 🙀 FAILED")

    def addError(self, test, err):
        super().addError(test, err)
        print(" 💥 ERROR")


def stop_process_group(proc, grace=20):
    """Arrete valgrind ET tous ses enfants (CGI, etc.) restes dans le groupe.

    Escalade SIGINT -> SIGTERM -> SIGKILL sur le *groupe* entier, pas juste
    sur valgrind : sinon un enfant CGI fige survit, garde le port ouvert et
    oblige a faire un SIGINT manuel au run suivant.
    """
    try:
        pgid = os.getpgid(proc.pid)
    except ProcessLookupError:
        return  # deja mort

    for sig, wait in ((signal.SIGINT, grace), (signal.SIGTERM, 5), (signal.SIGKILL, 5)):
        try:
            os.killpg(pgid, sig)
        except ProcessLookupError:
            break  # plus personne dans le groupe
        try:
            proc.wait(timeout=wait)
            if sig != signal.SIGKILL:
                try:
                    os.killpg(pgid, signal.SIGKILL)
                except ProcessLookupError:
                    pass
            return
        except subprocess.TimeoutExpired:
            print(f"Signal {sig} n'a pas suffi, escalade...")
            subprocess.run(["ps", "-ef"])

    proc.wait()


class EmojiRunner(unittest.TextTestRunner):
    #resultclass = EmojiTestResult
    resultclass = EmojiTestResult
    def run(self, test):
        result = self._makeResult()
        test(result)
        result.printErrors()
        return result

if __name__ == "__main__":

    print("Launching webserv under Valgrind...")

    cmd = [
        "valgrind",
        "--leak-check=full",
        "--show-leak-kinds=all",
        "--track-origins=yes",
        f"--log-file={VALGRIND_LOG}",
        "./webserv",
        "_bsugerTests/configs/default.conf"
    ]
    proc = subprocess.Popen(
        cmd,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        start_new_session=True
    )
    try:
        wait_for_server()
        loader = unittest.TestLoader()
        suite = loader.discover("_bsugerTests/tests/unittest_python")
        runner = EmojiRunner(stream=sys.stdout, verbosity=0)
        result = runner.run(suite)
    finally:
        print("\nStopping webserv...")
        stop_process_group(proc, grace=20)
        analyse_valgrind(VALGRIND_LOG)

    sys.exit(not result.wasSuccessful())
