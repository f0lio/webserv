import cgi, time, os

def main():
	form = cgi.FieldStorage()
	print("<p>Hello, world!</p>")
	print("<p>The time is: " + time.ctime() + "</p>")
	print("<html><body>")
	if not "name" in form:
		print("<h1>Hello!</h1>")
		print("<p>You did not give me your name!</p>")
	else:
		print("<h1>Hello, %s</h1>" % form["name"].value)
	print("</body></html>")

main()