import os;

method	= os.getenv('REQUEST_METHOD')
queries	= os.getenv('QUERY_STRING')

def	print_query():
	qs = queries.split('&')
	for q in qs:
		key, value = q.split('=')
		print(f"<p><strong>{key}</strong> = {value}</p>\n")

def main():
	print("<!DOCTYPE html>\n")
	print("<html lang=\"fr\">\n")
	print("<head><meta charset=\"utf-8\"><title>CGI Webserv</title></head>\n")
	print("<style>*{padding:0;margin:0;background-color:bisque;}.container{padding:10px;}body{text-align:center;}h1{color:goldenrod;padding:10px;}h2{color: maroon;}</style>\n")
	print("<body>\n")
	print("<h1>CGI Webserv (Python)</h1>\n")
	print("<h2>Welcome to the Fabulous CGI PHP of Minicube & Rico</h2>\n")
	print(f"<p><strong>Method :</strong>  {method} </p><br/>\n")
	print("<h2>Queries</h2>\n")
	print_query()
	print ("<hr>\n")
	print (f"<p><em>Server: . {os.getenv('SERVER_NAME')} . : . {os.getenv('SERVER_PORT')}. | Protocol: . {os.getenv('SERVER_PROTOCOL')} . </em></p>\n")
	print ("</body>\n</html>\n")

main()
