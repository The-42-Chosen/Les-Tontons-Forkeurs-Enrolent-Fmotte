import unittest
import requests
import socket

BASE_URL = "http://localhost:8002" 

class InvalidTransferEncodingTestWebservFunctional(unittest.TestCase):
    def test_invalid_chunk_size(self):
        """Invalid hexadecimal chunk size"""
        filename = "invalid_chunk.txt"
        request = (
            f"POST /cgi-bin/router.py?name={filename} HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "G\r\n"          # Taille invalide
            "Hello\r\n"
            "0\r\n"
            "\r\n"
        ).encode()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        data = b""
        try:
            s.connect(("localhost", 8002))
            s.sendall(request)
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
        except socket.timeout:
            pass
        finally:
            s.close()
        resp = data.decode("utf-8", errors="ignore")
        self.assertIn("HTTP/1.1 400", resp,
                      f"Malformed chunk must return 400.\n{resp}")
        response = requests.get(f"{BASE_URL}/upload/{filename}")
        self.assertEqual(
            response.status_code,
            404,
            "A malformed chunked request must not create the file via CGI."
        )

    def test_invalid_size_indication(self):
        """Invalid value received chunk value"""
        filename = "invalid_chunk_received.txt"
        request = (
            f"POST /cgi-bin/router.py?name={filename} HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "1\r\n"          # Taille invalide
            "Hello\r\n"
            "0\r\n"
            "\r\n"
        ).encode()
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        data = b""
        try:
            s.connect(("localhost", 8002))
            s.sendall(request)
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data += chunk
        except socket.timeout:
            pass
        finally:
            s.close()
        resp = data.decode("utf-8", errors="ignore")
        self.assertIn("HTTP/1.1 400", resp,
                      f"Malformed chunk must return 400.\n{resp}")
        response = requests.get(f"{BASE_URL}/upload/{filename}")
        self.assertEqual(
            response.status_code,
            404,
            "A malformed chunked request must not create the file via CGI."
        )

