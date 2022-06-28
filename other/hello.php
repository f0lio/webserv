<?php
	$name = $_POST['name'];
	// header('Content-Type: text/plain');
	if ($name == "") {
		echo "<h1>Hello, world!</h1>";
		echo "<h2>You didn't enter your name.</h2>";
	} else {
		echo "<h1>Hello $name</h1>";
	}
?>
