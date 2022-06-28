import cgi

def main():
	form = cgi.FieldStorage()
	print("Content-type: text/html\r\n")
	print("\r\n\r\n")
	print("<html><body>")
	if not "name" in form:
		print("<h1>Hello!</h1>")
		print("<p>You did not give me your name!</p>")
	else:
		print("<h1>Hello, %s</h1>" % form["name"].value)
	print("</body></html>")
