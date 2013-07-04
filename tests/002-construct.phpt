--TEST--
Test initialising the socket with incorrect args
--SKIPIF--
<?php require_once (dirname (__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

use NanoMsg\Nano as Nano;
use NanoMsg\Socket as NanoSocket;
use NanoMsg\Exception as NanoException;

try {
    $socket = new NanoSocket ();
    echo "FAIL";
} catch (NanoException $e) { echo "Exception" . PHP_EOL; }


try {
    $socket = new NanoSocket (500, 234123);
    echo "FAIL";
} catch (NanoException $e) { echo "Exception" . PHP_EOL; }

echo "OK";

--EXPECT--
Exception
Exception
OK