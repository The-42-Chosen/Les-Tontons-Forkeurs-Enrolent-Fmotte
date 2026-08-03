import unittest
import socket
import time

HOST = "localhost"
PORT = 8002

class ChunkedMaxBodySizeWebservFunctional(unittest.TestCase):
    def test_oversized_single_chunk_does_not_buffer(self):
        """Oversized chunk size is rejected without waiting body"""
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        data = b""
        closed = False
        try:
            s.connect((HOST, PORT))
            head = (
                "POST / HTTP/1.1\r\n"
                "Host: localhost:8002\r\n"
                "Transfer-Encoding: chunked\r\n"
                "Connection: close\r\n"
                "\r\n"
            ).encode()
            # 0x4C4B40 = 5_000_000 bytes
            s.sendall(head + b"4C4B40\r\nAAAAA")
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    closed = True
                    break
                data += chunk
        except socket.timeout:
            self.fail("Server hung waiting to buffer the oversized chunk body "
                      "(fix #1 regressed: the chunk-size limit is bypassed).")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()

        response = data.decode("utf-8", errors="ignore")
        self.assertTrue(response.startswith("HTTP/1.1"),
                        f"Server sent no HTTP response:\n{response!r}")
        self.assertTrue(closed,
                        "Server must close the connection after rejecting the "
                        "oversized chunk.")

    def test_chunk_within_limit_is_accepted(self):
        """A chunked body within the limit"""
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(3.0)
        data = b""
        try:
            s.connect((HOST, PORT))
            request = (
                "POST / HTTP/1.1\r\n"
                "Host: localhost:8002\r\n"
                "Transfer-Encoding: chunked\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "6\r\n"
                "Hello \r\n"
                "0\r\n"
                "\r\n"
            ).encode()
            s.sendall(request)
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
        except socket.timeout:
            self.fail("Timeout on a valid, in-limit chunked request.")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        finally:
            s.close()

        response = data.decode("utf-8", errors="ignore")
        self.assertTrue(response.startswith("HTTP/1.1"),
                        f"No valid HTTP response received:\n{response!r}")
        self.assertNotIn("HTTP/1.1 413", response,
                         f"An in-limit chunked body must not be rejected.\n{response}")

if __name__ == "__main__":
    unittest.main()
