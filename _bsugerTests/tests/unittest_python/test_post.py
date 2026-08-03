import unittest
import requests

BASE_URL = "http://localhost:8002" 

class PostTestWebservFunctional(unittest.TestCase):
    def test_post_index(self):
        """Test method POST on /"""
        body = {'somekey': 'somevalue'}
        response = requests.post(f"{BASE_URL}/", body)
        self.assertEqual(response.status_code, 200)
    def test_post_not_allow(self):
        """Test method POST on location (/test) where POST not allowed"""
        body = {'somekey': 'somevalue'}
        response = requests.post(f"{BASE_URL}/test", body)
        self.assertEqual(response.status_code, 405)
    def test_post_nonexisting(self):
        """Test method POST on something not existing"""
        body = {'somekey': 'somevalue'}
        response = requests.post(f"{BASE_URL}/asldfasfjlkh", body)
        self.assertEqual(response.status_code, 404)

if __name__ == "__main__":
    unittest.main()
