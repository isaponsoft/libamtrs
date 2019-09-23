<?php
$header_filename	= $argv[1];
$header				= file_get_contents($header_filename);
$lines				= [];
$basename			= pathinfo($header_filename)['filename'];

$fnames				= [];
$maxlen				= 0;
foreach (explode("\n", $header) as $line)
{
	$lines[] = trim($line);
	if (preg_match('/GLAPI[ \t]+(\w+) APIENTRY[ \t]+(\w+)[ \t]*\(([\w \t\*,]+)\)/', $line, $m))
	{
		$fnames[]	= $m[2];
		$len		= strlen($m[2]);
		if ($len > $maxlen)
		{
			$maxlen = $len;
		}
	}
}


$fp	= fopen("{$basename}-value.hpp", "w");
foreach ($fnames as $fname)
{
	$ftype	= strtoupper($fname);
	$space	= $maxlen + 2 - strlen($ftype);
	$fmt	= "PFN%sPROC%{$space}s%s;\n";
	fprintf($fp, $fmt, $ftype, " ", $fname);
}
fclose($fp);

$fp	= fopen("{$basename}-init.hpp", "w");
foreach ($fnames as $fname)
{
	$ftype	= strtoupper($fname);
	$space	= $maxlen + 2 - strlen($ftype);
	$fmt	= "%s%{$space}s = (PFN%sPROC)wglGetProcAddress(\"%s\");\n";
	fprintf($fp, $fmt, $fname, "", $ftype, $fname);
}
fclose($fp);
