import unittest
import socket
import time

HOST = "localhost"
PORT = 8002

class OversizedContentLengthWebservFunctional(unittest.TestCase):
    def _send_and_read(self, request_bytes, timeout=3.0):
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(timeout)
        try:
            s.connect((HOST, PORT))
            start = time.time()
            s.sendall(request_bytes)
            data = b""
            closed = False
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    closed = True
                    break
                data += chunk
            elapsed = time.time() - start
            return data, closed, elapsed
        except socket.timeout:
            self.fail("Server never answered: it is still waiting for the full "
                      "body (fix #2 regressed, the oversized body is buffered).")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()

    def test_oversized_content_length_immediate_413(self):
        """413 on Content-Length over the limit without sending the full body"""
        request = (
            "POST /upload HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 100000\r\n"
            "\r\n"
        ).encode() + b"0123456789"

        data, closed, _ = self._send_and_read(request)
        response = data.decode("utf-8", errors="ignore")

        self.assertIn("HTTP/1.1 413", response,
                      f"Expected 413 Payload Too Large.\n{response}")
        self.assertIn("Connection: close", response,
                      f"An oversized-body error must close the connection.\n{response}")
        self.assertTrue(closed,
                        "Server must close the connection after the 413.")

    def test_body_within_limit_not_rejected(self):
        """A body within client_max_body_size is ok"""
        body = b"A" * 50
        request = (
            "POST / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Content-Type: text/plain\r\n"
            f"Content-Length: {len(body)}\r\n"
            "Connection: close\r\n"
            "\r\n"
        ).encode() + body

        data, _, _ = self._send_and_read(request)
        response = data.decode("utf-8", errors="ignore")

        self.assertTrue(response.startswith("HTTP/1.1"),
                        f"No valid HTTP response received:\n{response!r}")
        self.assertNotIn("HTTP/1.1 413", response,
                         f"An in-limit body must not be rejected as 413.\n{response}")


if __name__ == "__main__":
    unittest.main()
