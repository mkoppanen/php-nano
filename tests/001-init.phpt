--TEST--
Test initialising the socket
--SKIPIF--
<?php require_once (dirname (__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

use NanoMsg\Nano as Nano;
use NanoMsg\Socket as NanoSocket;
use NanoMsg\Exception as NanoException;

try {
    $socket = new NanoSocket (Nano::AF_SP, Nano::NN_PUB);
} catch (NanoException $e) {
    echo "Failed: " . $e->getMessage ();
}

echo "OK";

--EXPECT--
OK