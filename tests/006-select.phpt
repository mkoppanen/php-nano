--TEST--
Test using SNDFD and RCVFD in php socket select
--SKIPIF--
<?php require_once (dirname (__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

use NanoMsg\Nano as Nano;
use NanoMsg\Socket as NanoSocket;
use NanoMsg\Exception as NanoException;

$pub = new NanoSocket (Nano::AF_SP, Nano::NN_PUB);
$pub->bind ("inproc://hello");

$sub = new NanoSocket (Nano::AF_SP, Nano::NN_SUB);
$sub->connect ("inproc://hello");
$sub->setSockOpt (Nano::NN_SUB, Nano::NN_SUB_SUBSCRIBE, "hello");

// Get the receive fd
$rcvfd = $sub->getSockOpt (Nano::NN_SOL_SOCKET, Nano::NN_RCVFD);
var_dump ($rcvfd);

// and the send fd
$sndfd = $sub->getSockOpt (Nano::NN_SOL_SOCKET, Nano::NN_SNDFD);
var_dump ($sndfd);

$read = array ($rcvfd, $sndfd);
$write = array ();
$except = array ();

// Make sure that pub can send
$num = stream_select ($read, $write, $except, 1);
var_dump ($num);

// Send a message
$pub->send ("hello there");

// Make sure that pub is writable and sub is readable
$read = array ($rcvfd);
$num = stream_select ($read, $write, $except, 1);
var_dump ($num);

echo "OK";

--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
int(1)
int(1)
OK