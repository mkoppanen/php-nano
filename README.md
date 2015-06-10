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
10. Reload fpm service


Base classes and methods
=========================



