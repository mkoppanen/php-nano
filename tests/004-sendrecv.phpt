--TEST--
Test sending and receiving
--SKIPIF--
<?php require_once (dirname (__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

use NanoMsg\Nano as Nano;
use NanoMsg\Socket as NanoSocket;
use NanoMsg\Exception as NanoException;

$peer1 = new NanoSocket (Nano::AF_SP, Nano::NN_PAIR);
$peer1->bind ("inproc://hello");

$peer2 = new NanoSocket (Nano::AF_SP, Nano::NN_PAIR);
$peer2->connect ("inproc://hello");

$peer1->send ("peer1 to peer2");
var_dump ($peer2->recv ());

$peer2->send ("peer2 to peer1");
var_dump ($peer1->recv ());

echo "OK";

--EXPECTF--
string(14) "peer1 to peer2"
string(14) "peer2 to peer1"
OK