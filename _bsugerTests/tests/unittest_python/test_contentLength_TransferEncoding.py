import unittest
import requests
import socket

BASE_URL = "http://localhost:8002" 

class ContentLengthWebservFunctional(unittest.TestCase):
    def test_get_index(self):
        """Test Content length too long compare to the configuration file"""
        body = "A" * 201
        response = requests.post(f"{BASE_URL}/", data=body, headers={"Content-Type": "text/plain"})
        self.assertEqual(response.status_code, 413)

    def test_post_conflict_chunked_and_content_length(self):
        """Test Conflict Content Length and Transfer Encoding chunked"""
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
            "Content-Length: 50\r\n"
            "Transfer-Encoding: chunked\r\n"
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
            self.assertTrue(
                "HTTP/1.1 400" in response,
                f"No valid HTTP response received:\n{response}"
            )
        except socket.timeout:
            self.fail("Timeout on chunked+content-length request")
        except ConnectionRefusedError:
            self.fail("Connection refused")
        finally:
            s.close()




if __name__ == "__main__":
    unittest.main()
