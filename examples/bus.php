<?php
/**
 * run like this
 * 
 php -d extension=nano.so  ./bus.php node0 node0 node1 node2 & node0=$!
 php -d extension=nano.so  ./bus.php node1 node1 node2 node3 & node1=$!
 php -d extension=nano.so  ./bus.php node2 node2 node3 & node2=$!
 php -d extension=nano.so  ./bus.php node3 node3 node0 & node3=$!
 sleep 5
 kill $node0 $node1 $node2 $node3
 */

use NanoMsg\Socket as NanoSocket;
use NanoMsg\Nano as Nano;

if ($argc < 4) {
    echo "Usage: php ./bus.php nodename peerurl peerurl [peerurl...]\n";
    die(1);
}
$name      = $argv[1];
$node      = $argv[2];
$listNode   = array();
for ($x=3; $x< $argc; $x++) {
    $listNode[] = $argv[$x];
}
//bus example from http://tim.dysinger.net/posts/2013-09-16-getting-started-with-nanomsg.html

$socket = new NanoSocket (Nano::AF_SP, Nano::NN_BUS);


//bind to 'node' for sending out
// node is the exact same as 'name' in this demo
$socket->bind("ipc:///tmp/".$node.".ipc");

foreach ($listNode as $_n) {
    //connect to peers listed as args 3, 4, 5, ...
    $socket->connect("ipc:///tmp/". $_n.".ipc");
}

//receive timeout is 100ms
$socket->setSockOpt(Nano::NN_SOL_SOCKET, Nano::NN_RCVTIMEO, 100);
//wait for connections (in a real application this could be done with handshaking)
sleep(1);

//broadcast to the bound endpoint our name
echo sprintf("SENDING '%s' ONTO BUS\n", $name);
$socket->send($name);

$c = 0;
while (true) {
    $c++;
    try {
        //recv from connected endpoints what they have placed ont he bus
        $recv = $socket->recv ();
        echo sprintf("%s: RECEIVED '%s' FROM BUS\n", $name, $recv);
    } catch(Exception $e) {
        //we try 5 times, but maybe we only send 2 or 3 times
        //ignore
    }
    if ($c >=5) break;
}
