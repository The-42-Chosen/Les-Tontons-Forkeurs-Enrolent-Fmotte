import unittest
import socket

HOST = "localhost"
PORT = 8002

def read_one_response(sock):
    buf = b""
    while b"\r\n\r\n" not in buf:
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            return buf.decode(errors="ignore"), False
        if not chunk:
            return buf.decode(errors="ignore"), True
        buf += chunk
    head, _, body = buf.partition(b"\r\n\r\n")
    cl = 0
    for line in head.split(b"\r\n")[1:]:
        if line.lower().startswith(b"content-length:"):
            try:
                cl = int(line.split(b":", 1)[1].strip())
            except ValueError:
                cl = 0
    closed = False
    while len(body) < cl:
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            break
        if not chunk:
            closed = True
            break
        body += chunk
    return (head + b"\r\n\r\n" + body).decode(errors="ignore"), closed


class ChunkErrorRecoveryWebservFunctional(unittest.TestCase):

    def test_malformed_chunk_returns_400(self):
        """A malformed chunk size is answered with 400, not 200"""
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        try:
            s.connect((HOST, PORT))
            # 'G' is not a valid hexadecimal chunk size.
            s.sendall(
                b"POST / HTTP/1.1\r\n"
                b"Host: localhost:8002\r\n"
                b"Transfer-Encoding: chunked\r\n"
                b"Connection: close\r\n"
                b"\r\n"
                b"G\r\nHello\r\n0\r\n\r\n"
            )
            resp, _ = read_one_response(s)
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()

        self.assertIn("HTTP/1.1 400", resp,
                      f"A malformed chunk must return 400, not 200.\n{resp}")

    def test_malformed_chunk_closes_even_on_keep_alive(self):
        """A malformed chunk closes the connection even without Connection: close"""
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        try:
            s.connect((HOST, PORT))
            s.sendall(
                b"POST / HTTP/1.1\r\n"
                b"Host: localhost:8002\r\n"
                b"Transfer-Encoding: chunked\r\n"
                b"\r\n"
                b"G\r\nHello\r\n0\r\n\r\n"
            )
            resp, _ = read_one_response(s)
            self.assertIn("Connection: close", resp,
                          f"A chunk error must force Connection: close.\n{resp}")
            try:
                extra = s.recv(4096)
            except socket.timeout:
                self.fail("Server kept the connection open after a chunk error "
                          "(parser wedged / not closed).")
            self.assertEqual(extra, b"",
                             "Server must close the connection after a chunk error.")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()

if __name__ == "__main__":
    unittest.main()
