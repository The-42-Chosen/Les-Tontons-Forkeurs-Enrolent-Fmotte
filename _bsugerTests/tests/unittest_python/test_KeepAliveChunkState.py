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


class KeepAliveChunkStateWebservFunctional(unittest.TestCase):

    def test_trailer_reset_between_requests(self):
        """Two keep-alive chunked requests each with a trailer both succeed"""
        req = (
            b"POST / HTTP/1.1\r\n"
            b"Host: localhost:8002\r\n"
            b"Transfer-Encoding: chunked\r\n"
            b"\r\n"
            b"6\r\nHello \r\n"
            b"0\r\nX-Trailer: a\r\n\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        try:
            s.connect((HOST, PORT))

            s.sendall(req)
            r1, c1 = read_one_response(s)
            self.assertIn("HTTP/1.1 200", r1,
                          f"1st chunked+trailer request must be 200.\n{r1}")
            self.assertFalse(c1, "Connection should stay alive after the 1st request.")

            s.sendall(req)
            r2, c2 = read_one_response(s)
            self.assertIn("HTTP/1.1 200", r2,
                          f"2nd chunked+trailer request must also be 200 "
                          f"(Buffer._trailers leaked -> spurious error).\n{r2}")
            self.assertFalse(c2, "Connection should stay alive after the 2nd request.")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()

    def test_chunk_substate_reset_after_nonchunked(self):
        """chunked -> non-chunked -> chunked on one connection all succeed"""
        chunked = (
            b"POST / HTTP/1.1\r\n"
            b"Host: localhost:8002\r\n"
            b"Transfer-Encoding: chunked\r\n"
            b"\r\n"
            b"6\r\nHello \r\n0\r\n\r\n"
        )
        nonchunked = (
            b"POST / HTTP/1.1\r\n"
            b"Host: localhost:8002\r\n"
            b"Content-Length: 5\r\n"
            b"\r\n"
            b"AAAAA"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        try:
            s.connect((HOST, PORT))

            s.sendall(chunked)
            r1, _ = read_one_response(s)
            self.assertIn("HTTP/1.1 200", r1, f"1st (chunked) must be 200.\n{r1}")

            s.sendall(nonchunked)
            r2, _ = read_one_response(s)
            self.assertIn("HTTP/1.1 200", r2, f"2nd (non-chunked) must be 200.\n{r2}")

            s.sendall(chunked)
            r3, _ = read_one_response(s)
            self.assertIn("HTTP/1.1 200", r3,
                          f"3rd (chunked) must be 200 (Buffer._buffer sub-state "
                          f"leaked from req1 -> mis-parse).\n{r3}")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()


if __name__ == "__main__":
    unittest.main()
