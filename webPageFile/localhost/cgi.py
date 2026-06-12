import os;

method = os.getenv('REQUEST_METHOD')

def main():
	print("<!DOCTYPE html>\n")
	print("<html lang=\"fr\">\n")
	print("<head><meta charset=\"utf-8\"><title>CGI Webserv</title></head>\n")
	print("<style>*{padding:0;margin:0;background-color:bisque;}.container{padding:10px;}body{text-align:center;}h1{color:goldenrod;padding:10px;}h2{color:maroon;padding:10px;}</style>\n")
	print("<body>\n")
	print("<h1>CGI Webserv (Python)</h1>\n")
	print("<h2> Welcome to the Fabulous CGI PHP of Minicube & Rico</p>\n")
	print(f"<p><strong>Method :</strong>  {method} </p>\n")
    # return 0

main()
