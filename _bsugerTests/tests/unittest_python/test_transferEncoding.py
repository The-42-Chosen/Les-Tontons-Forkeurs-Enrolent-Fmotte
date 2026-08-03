import unittest
import requests
import socket


class TransferEncodingWebservFunctional(unittest.TestCase):
    def test_post_transfer_encoding_non_supported(self):
        """Test other Transfer-Encoding non supported"""
        body = (
            "aaaaa"
        )
        request = (
            "POST / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Content-Type: text/plain\r\n"
            "Transfer-Encoding: gzip\r\n"
            "Content-Length: 5\r\n"
            "Connection: close\r\n"
            "\r\n"
        ).encode() + body.encode()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request)
            data = b""
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
            response = data.decode("utf-8", errors="ignore")
            if not response:
                self.fail("No response received from server")
            self.assertIn(
                "HTTP/1.1 400",
                response,
                f"Invalid response:\n{response}"
            )
        except socket.timeout:
            self.fail("Timeout on gzip transfer-encoding")
        except ConnectionRefusedError:
            self.fail("Connection refused")
        finally:
            s.close()

    def test_post_TransferEncoding_andContentLength(self):
        """Test invalid Header with Transfer Encoding and Content Lenght"""
        body = (
            "5\r\n"
            "Hello\r\n"
            "6\r\n"
            " World\r\n"
            "0\r\n"
            "\r\n"
        )
        request = (
            "POST / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Content-Type: text/plain\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Length: 5\r\n"
            "Connection: close\r\n"
            "\r\n"
        ).encode() + body.encode()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request)
            data = b""
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
            response = data.decode("utf-8", errors="ignore")
            if not response:
                self.fail("No response received from server")
            self.assertIn(
                "HTTP/1.1 400",
                response,
                f"Invalid response:\n{response}"
            )
        except socket.timeout:
            self.fail("Timeout on content-length + transfer-encoding test")
        except ConnectionRefusedError:
            self.fail("Connection refused")
        finally:
            s.close()
    def test_post_TransferEncoding_CGI(self):
        """Transfer Encoding CGI test if valid """
        request = (
            "POST /cgi-bin/router.py?name=test_cgi_post.txt HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Content-Type: text/plain\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Connection: close\r\n"
            "\r\n"
            "6\r\n"
            "Hello \r\n"
            "6\r\n"
            " World\r\n"
            "0\r\n"
            "\r\n"
        ).encode()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request)
            data = b""
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
            response = data.decode("utf-8", errors="ignore")
            if not response:
                self.fail("No response received from server")
            self.assertIn(
                "HTTP/1.1 201",
                response,
                f"Invalid response:\n{response}"
            )
        except socket.timeout:
            self.fail("Timeout transfer-encoding CGI")
        except ConnectionRefusedError:
            self.fail("Connection refused")
        finally:
            s.close()



if __name__ == "__main__":
    unittest.main()
