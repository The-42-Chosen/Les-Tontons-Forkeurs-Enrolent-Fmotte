import unittest
import requests

BASE_URL = "http://localhost:8002" 

class GetTestWebservFunctional(unittest.TestCase):
    def test_get_index(self):
        """Test method GET on /"""
        response = requests.get(f"{BASE_URL}/")
        self.assertEqual(response.status_code, 200)
    def test_get_upload(self):
        """Test method GET on /upload"""
        response = requests.get(f"{BASE_URL}/upload")
        self.assertEqual(response.status_code, 200)
    def test_get_return(self):
        """Test method GET on /forbidden return 599 (random number)"""
        response = requests.get(f"{BASE_URL}/forbidden")
        self.assertEqual(response.status_code, 599)
    '''
    def test_get_red(self):
        """Test method GET on /redirection to /upload"""
        response = requests.get(f"{BASE_URL}/redirection")
        self.assertEqual(response.status_code, 200)
    '''
    def test_get_nonexisting(self):
        """Test method GET on something not existing"""
        response = requests.get(f"{BASE_URL}/asldfasfjlkh")
        self.assertEqual(response.status_code, 404)
    def test_get_notauthorize(self):
        """Test method GET on a location without GET method allowed"""
        response = requests.get(f"{BASE_URL}/test")
        self.assertEqual(response.status_code, 405)

if __name__ == "__main__":
    unittest.main()
