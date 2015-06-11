[![Build Status](https://travis-ci.org/mkoppanen/php-nano.png?branch=master)](https://travis-ci.org/mkoppanen/php-nano)

nanomsg extension for PHP
=========================

For more information about nanomsg see: http://nanomsg.org/

How to install
=========================

1. Install nanomsg library, see: https://github.com/nanomsg/nanomsg
2. Install PHP dev tools for extension:  sudo apt-get install php5-dev (for Debian/Ubuntu) or sudo yum install php-devel (Fedora) 
3. Clone repo at your server, e.g. cd /var/tmp && git clone https://github.com/nanomsg/nanomsg
4. cd /var/tmp/php-nano
5. phpize   
6. configure --enable-nano
7. make
8. make install
9. Edit your php.ini file, add extension=nano.so 
10. Restart FPM service or Apache web server.


!Note! tested with PHP 5.6.2 only ( PHP_VERSION : 5.6.9-0+deb8u1  ZEND_VERSION: 2.6.0  ). PHP must support Namespace (PHP 5 >= 5.3.0, see: http://php.net/manual/en/language.namespaces.php)

Base classes and methods
=========================

php-nano exposed namespace NanoMsg with three class:

* NanoMsg\Nano - main class
* NanoMsg\Socket - base socket class
* NanoMsg\Exception - utility class for Exception

For example, use it with short name:

```php
use NanoMsg\Nano as Nano;
use NanoMsg\Socket as NanoSocket;
use NanoMsg\Exception as NanoException;
```

NanoMsg\Nano
=========================

Base class. Provide static function device (see: http://nanomsg.org/v0.5/nn_device.3.html)

Also provided some static constant:

domain of creating socket (see: http://nanomsg.org/v0.5/nn_socket.3.html)
* Nano::AF_SP - Standard full-blown SP socket
* Nano::AF_SP_RAW - Raw SP socket. Raw sockets omit the end-to-end functionality found in AF_SP sockets and thus can be used to implement intermediary devices in SP topologies.

Protocol parameter defines the type of the socket, which in turn determines the exact semantics of the socket

* Nano::NN_PAIR (see: http://nanomsg.org/v0.5/nn_pair.7.html) Pair protocol is the simplest and least scalable scalability protocol. It allows scaling by breaking the application in exactly two pieces
* Nano::NN_REQ  and ::NN_REQ (see: http://nanomsg.org/v0.5/nn_reqrep.7.html) This protocol is used to distribute the workload among multiple stateless workers
* Nano::NN_PUB and Nano::NN_SUB (see: http://nanomsg.org/v0.5/nn_pubsub.7.html) Broadcasts messages to multiple destinations.
* Nano::NN_SURVEYOR and Nano::NN_RESPONDENT (see: http://nanomsg.org/v0.5/nn_survey.7.html)
* Nano::NN_PUSH and Nano::NN_PULL (see: http://nanomsg.org/v0.5/nn_pipeline.7.html)
* Nano::NN_BUS (see: http://nanomsg.org/v0.5/nn_bus.7.html)

Nanomsg library support 4 network/communication protocol at now: 

* inproc (e.g.: inproc://example) - for in-process low-latency communication at same machine.
* ipc (e.g.: ipc://example) - for inter-process communication at same machine. 
* tcp (e.g.: tcp://127.0.0.1:8081) - for tcp communication over LAN/WAN.
* ws (e.g.: ws://127.0.0.1:8081) - use WebSocket protocol over TCP. Current release, 0.5.х not supported WS, only latest build from master. 

Performance note: ipc vs inproc at same machine has 2х - 4х slow (tested NN_PAIR socket), tcp vs ipc 3х - 5х slow, and ws same as tcp at performance. 

For tcp and ws protocol, for bind method, valid IP (not domain name) and port required. For connect - you may use domain name as adress. More see: http://nanomsg.org/v0.5/nn_tcp.7.html



NanoMsg\Socket
=========================

Main class to provide socket.

methods:
* bind
* connect
* shutdown
* send
* recv
* setsockopt
* getsockopt

