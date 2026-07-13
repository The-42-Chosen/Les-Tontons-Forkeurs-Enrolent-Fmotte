import os;
import sys;
from urllib.parse import parse_qsl
from html import escape

method	= os.getenv('REQUEST_METHOD') or ''
queries	= os.getenv('QUERY_STRING') or ''

def	get_cookie(name):
	raw = os.getenv('HTTP_COOKIE') or ''
	for part in raw.split(';'):
		if '=' in part:
			key, value = part.split('=', 1)
			if key.strip() == name:
				return value.strip()
	return ''

# Compteur de visites propre a ce CGI (cookie scope sur /cgi.py)
previous	= get_cookie('hits_py')
count		= int(previous) + 1 if previous.isdigit() else 1

def	read_post_body():
	if method != 'POST':
		return ''
	try:
		length = int(os.getenv('CONTENT_LENGTH') or 0)
	except ValueError:
		length = 0
	if length <= 0:
		return ''
	return sys.stdin.read(length)

def	print_params(raw):
	pairs = parse_qsl(raw, keep_blank_values=True)
	if not pairs:
		print("<p>No parameters</p>\n")
		return
	for key, value in pairs:
		print(f"<p><strong>{escape(key)}</strong> = {escape(value)}</p>\n")

def main():
	body = read_post_body()

	print("Content-Type: text/html")
	print(f"Set-Cookie: hits_py={count}; Path=/cgi.py")
	print()

	print("<!DOCTYPE html>\n")
	print("<html lang=\"fr\">\n")
	print("<head><meta charset=\"utf-8\"><title>CGI Webserv</title></head>\n")
	print("<style>*{padding:0;margin:0;background-color:bisque;}.container{padding:10px;}body{text-align:center;}h1{color:goldenrod;padding:10px;}h2{color: maroon;}</style>\n")
	print("<body>\n")
	print("<h1>CGI Webserv (Python)</h1>\n")
	print("<h2>Welcome to the Fabulous CGI PHP of Minicube & Rico</h2>\n")
	print(f"<h2>Visites (Python) : {count}</h2>\n")
	print(f"<p><strong>Method :</strong>  {escape(method)} </p><br/>\n")
	print("<h2>Queries (GET)</h2>\n")
	print_params(queries)
	print("<h2>Body (POST)</h2>\n")
	print_params(body)
	print("<hr>\n")
	print("<div class=\"container\">\n")
	print("<h2>Test GET /cgi.py</h2>\n")
	print("<form method=\"GET\" action=\"/cgi.py\">\n")
	print("<label for=\"get-msg\">Message :</label>\n")
	print("<input id=\"get-msg\" type=\"text\" name=\"message\">\n")
	print("<button type=\"submit\">Send (GET)</button>\n")
	print("</form>\n")
	print("</div>\n")
	print("<div class=\"container\">\n")
	print("<h2>Test POST /cgi.py</h2>\n")
	print("<form method=\"POST\" action=\"/cgi.py\" enctype=\"application/x-www-form-urlencoded\">\n")
	print("<label for=\"post-msg\">Message :</label>\n")
	print("<input id=\"post-msg\" type=\"text\" name=\"message\">\n")
	print("<button type=\"submit\">Send (POST)</button>\n")
	print("</form>\n")
	print("</div>\n")
	print("<hr>\n")
	print (f"<p><em>Server: . {os.getenv('SERVER_NAME')} . : . {os.getenv('SERVER_PORT')}. | Protocol: . {os.getenv('SERVER_PROTOCOL')} . </em></p>\n")
	print ("</body>\n</html>\n")

main()
