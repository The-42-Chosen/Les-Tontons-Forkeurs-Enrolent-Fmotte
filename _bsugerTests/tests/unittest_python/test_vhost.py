import unittest
import requests

class VhostTestWebservFunctional(unittest.TestCase):
    def test_vhost_example(self):
        """Test vhost www.example.com sur le port 8003"""
        headers = {"Host": "www.example.com"}
        response = requests.get("http://127.0.0.1:8003/", headers=headers)
        
        self.assertEqual(response.status_code, 200)
        self.assertIn("Les Tontons Forkeurs", response.text) 

    def test_vhost_42(self):
        """Test vhost www.42.fr sur le port 8003"""
        headers = {"Host": "www.42.fr"}
        response = requests.get("http://127.0.0.1:8003/", headers=headers)
        self.assertEqual(response.status_code, 200)
        self.assertIn("Ici, c'est Paris !", response.text) 
