import unittest
import socket

BASE_URL = "http://localhost:8002" 

class InvalidTestWebservFunctional(unittest.TestCase):
    def test_NotImplemented(self):
        """ Method Not Implemented"""
        request_pure = (
            "PUT / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure.encode("utf-8"))
            data_received = s.recv(4096).decode("utf-8")
            s.close()
            #print(f"\nRéponse brute reçue:\n{data_received}\n" + "-"*40)
            if not data_received:
                self.fail("Webserv closed the socket")
            self.assertIn("HTTP/1.1 501", data_received, 
                          f"Server should have sent 501 . We received :\n{data_received}")
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")

    def test_invalidHTTPVersion(self):
        """Invalid HTTP Version"""
        request_pure = (
            "GET / HTTP/3.3\r\n"
            "Host: localhost:8002\r\n"
            "\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure.encode("utf-8"))
            data_received = s.recv(4096).decode("utf-8")
            s.close()
            #print(f"\nRéponse brute reçue:\n{data_received}\n" + "-"*40)
            if not data_received:
                self.fail("Webserv closed the socket")
            self.assertIn("HTTP/1.1 505", data_received, 
                          f"Server should have sent 505 Bad Request. We received :\n{data_received}")
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
    def test_missing_Host(self):
        """No Host in the request"""
        request_pure = (
            "GET / HTTP/3.3\r\n"
            "\r\n\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure.encode("utf-8"))
            data_received = s.recv(4096).decode("utf-8")
            s.close()
            #print(f"\nRéponse brute reçue:\n{data_received}\n" + "-"*40)
            if not data_received:
                self.fail("Webserv closed the socket")
            self.assertIn("HTTP/1.1 505", data_received, 
                          f"Server should have sent 505 Bad Request. We received :\n{data_received}")
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")


if __name__ == "__main__":
    unittest.main()
