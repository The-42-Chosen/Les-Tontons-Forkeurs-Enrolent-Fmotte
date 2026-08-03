import unittest
import requests

BASE_URL = "http://localhost:8002" 

class DeleteTestWebservFunctional(unittest.TestCase):
    def test_delete_index(self):
        """Test method DELETE on /"""
        response = requests.delete(f"{BASE_URL}/")
        self.assertEqual(response.status_code, 200)
    def test_delete_not_allow(self):
        """Test method DELETE on location (/test2) where DELETE not allowed"""
        response = requests.delete(f"{BASE_URL}/test2")
        self.assertEqual(response.status_code, 405)
    def test_delete_nonexisting(self):
        """Test method DELETE on something not existing"""
        response = requests.delete(f"{BASE_URL}/asldfasfjlkh")
        self.assertEqual(response.status_code, 404)

if __name__ == "__main__":
    unittest.main()
