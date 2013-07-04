--TEST--
Test connecting and binding the socket
--SKIPIF--
<?php require_once (dirname (__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

use NanoMsg\Nano as Nano;
use NanoMsg\Socket as NanoSocket;
use NanoMsg\Exception as NanoException;

$socket = new NanoSocket (Nano::AF_SP, Nano::NN_PUB);
$bind_eid = $socket->bind ("inproc://hello");
$connect_eid = $socket->connect ("inproc://hello");

var_dump ($bind_eid);
var_dump ($connect_eid);

var_dump ($socket->shutdown ($bind_eid));
var_dump ($socket->shutdown ($connect_eid));

try {
    $socket->shutdown (42);
} catch (NanoException $e) {
    echo "Exception". PHP_EOL;
}
echo "OK";

--EXPECTF--
int(%d)
int(%d)
bool(true)
bool(true)
Exception
OK