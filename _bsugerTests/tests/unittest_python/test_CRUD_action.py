import unittest
import requests
import socket

BASE_URL = "http://localhost:8002" 

class CRUDTestWebservFunctional(unittest.TestCase):

    def test_a_cgi_post_pure_socket(self):
            """Test CGI POST create a file"""
            body = "Fichier cree par le test unitaire via post.py"
            body_bytes = body.encode("utf-8")
            request_pure = (
                "POST /cgi-bin/router.py?name=test_cgi_post.txt HTTP/1.1\r\n"
                "Host: localhost:8002\r\n"
                "Content-Type: text/plain\r\n"
                f"Content-Length: {len(body_bytes)}\r\n"
                "Connection: close\r\n"
                "\r\n"
            ).encode("utf-8") + body_bytes
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.settimeout(2.0)
            try:
                s.connect(("localhost", 8002))
                s.sendall(request_pure)
                data_received = s.recv(4096).decode("utf-8")
                s.close()
                if not data_received:
                    self.fail("Webserv closed the socket without responding.")
                self.assertIn("HTTP/1.1 201", data_received, 
                              f"CGI did not return 201 Created. Check the dump above.")
                self.assertIn("Created test_cgi_post.txt", data_received)
            except socket.timeout:
                s.close()
                self.fail("Webserv Timeout on CGI socket test")
            except ConnectionRefusedError:
                self.fail("Webserv Connection refused")
    def test_b_cgi_get_uploaded_file(self):
            """Test retrieving the newly created file via GET"""
            file_url = f"{BASE_URL}/upload/test_cgi_post.txt"
            try:
                response = requests.get(file_url, timeout=2.0)
                self.assertEqual(response.status_code, 200, 
                                 f"GET failed. Status: {response.status_code}. Response: {response.text}")
                expected_content = "Fichier cree par le test unitaire via post.py"

                self.assertEqual(response.text, expected_content, 
                             f"File content mismatch.\n"
                             f"Expected: '{expected_content}'\n"
                             f"Received: '{response.text}'")
            except requests.exceptions.RequestException as e:
                self.fail(f"Request to GET uploaded file failed: {e}")

    def test_c_cgi_delete_pure_socket(self):
        """Test CGI DELETE remove a file"""
        request_pure = (
            "DELETE /cgi-bin/router.py?name=test_cgi_post.txt HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Connection: close\r\n"
            "\r\n"
        ).encode("utf-8")
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure)
            data_received = b""
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                data_received += chunk
            s.close()
            response = data_received.decode("utf-8")
            #print(response)
            if not response:
                self.fail("Webserv closed the socket without responding.")
            self.assertIn(
                "HTTP/1.1 204",
                response,
                f"CGI did not return 204 OK. Check the dump above."
            )
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout on CGI DELETE test")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")

if __name__ == "__main__":
    unittest.main()
