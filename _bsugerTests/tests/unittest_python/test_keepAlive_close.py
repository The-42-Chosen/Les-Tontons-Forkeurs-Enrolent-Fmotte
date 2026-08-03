import unittest
import socket

BASE_URL = "http://localhost:8002" 

class KeepAliveClosedTestWebservFunctional(unittest.TestCase):
    def test_keep_alive(self):
        """Send Keep-Alive"""
        request_pure = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure.encode("utf-8"))
            data_received = s.recv(4096).decode("utf-8")
            if not data_received:
                self.fail("Webserv closed the socket immediately after responding.")
            self.assertIn("HTTP/1.1 200", data_received, f"Server did not respond with 200 OK. Received:\n{data_received}")
            self.assertIn("Connection: keep-alive", data_received, "Server response missing 'Connection: keep-alive' header")
            try:
                extra_data = s.recv(1024)
                if not extra_data:
                    self.fail("Webserv closed the connection right after sending the response despite Keep-Alive.")
                else:
                    self.fail(f"Webserv sent unexpected extra data after the response: {extra_data.decode('utf-8')}")
            except socket.timeout:
                pass
            s.close()
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout on the initial request")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")

    def test_connection_close_request(self):
        """Send Close connection"""
        request_pure = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure.encode("utf-8"))
            data_received = s.recv(4096).decode("utf-8")
            if not data_received:
                self.fail("Webserv closed the socket without sending any data.")
            self.assertIn("HTTP/1.1 200", data_received, f"Server did not respond with 200 OK. Received:\n{data_received}")
            self.assertIn("Connection: close", data_received, "Server response missing 'Connection: close' header")
            try:
                extra_data = s.recv(1024)
                if extra_data:
                    self.fail(f"Webserv sent unexpected extra data after the response: {extra_data.decode('utf-8')}")
            except socket.timeout:
                self.fail("Webserv kept the connection open (timeout) despite 'Connection: close' header.")
            s.close()
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout on the initial request")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")

    def test_KeepALiveFollowedClose(self):
        """Same Client Keep Alive then Close"""
        first_request = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
        )
        second_request = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Connection: close\r\n"
            "\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(first_request.encode("utf-8"))
            data_received_1 = s.recv(4096).decode("utf-8")
            if not data_received_1:
                self.fail("Webserv closed the socket after the first request.")
            self.assertIn("HTTP/1.1 200", data_received_1, f"First request failed. Received:\n{data_received_1}")
            s.sendall(second_request.encode("utf-8"))
            data_received_2 = s.recv(4096).decode("utf-8")
            if not data_received_2:
                self.fail("Webserv closed the socket before answering the second request.")
            self.assertIn("HTTP/1.1 200", data_received_2, f"Second request failed. Received:\n{data_received_2}")
            s.close()
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout during sequential keep-alive requests")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        except BrokenPipeError:
            s.close()
            self.fail("Webserv closed the connection prematurely (Broken Pipe) on the second request.")

    def test_keep_alive_both_requests(self):
        """Same client Keep Alive 2 times (fail first try Webserv, did not test valgrind back in the day)"""
        request_pure = (
            "GET / HTTP/1.1\r\n"
            "Host: localhost:8002\r\n"
            "Connection: keep-alive\r\n"
            "\r\n"
        )
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1.5)
        try:
            s.connect(("localhost", 8002))
            s.sendall(request_pure.encode("utf-8"))
            data_received_1 = s.recv(4096).decode("utf-8")
            if not data_received_1:
                self.fail("Webserv closed the socket after the first request.")
            self.assertIn("HTTP/1.1 200", data_received_1, f"First request failed. Received:\n{data_received_1}")
            self.assertIn("Connection: keep-alive", data_received_1, "First response missing 'Connection: keep-alive'")
            s.sendall(request_pure.encode("utf-8"))
            data_received_2 = s.recv(4096).decode("utf-8")
            if not data_received_2:
                self.fail("Webserv closed the socket before answering the second request.")
            self.assertIn("HTTP/1.1 200", data_received_2, f"Second request failed. Received:\n{data_received_2}")
            self.assertIn("Connection: keep-alive", data_received_2, "Second response missing 'Connection: keep-alive'")
            try:
                extra_data = s.recv(1024)
                if not extra_data:
                    self.fail("Webserv closed the connection right after the second response instead of keeping it alive.")
                else:
                    self.fail(f"Webserv sent unexpected extra data: {extra_data.decode('utf-8')}")
            except socket.timeout:
                pass
            s.close()
        except socket.timeout:
            s.close()
            self.fail("Webserv Timeout during double keep-alive requests")
        except ConnectionRefusedError:
            self.fail("Webserv Connection refused")
        except BrokenPipeError:
            s.close()
            self.fail("Webserv closed the connection prematurely (Broken Pipe) on the second request.")
