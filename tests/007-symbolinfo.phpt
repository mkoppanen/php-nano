--TEST--
Test symbol info
--SKIPIF--
<?php require_once (dirname (__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php

use NanoMsg\Nano as Nano;
use NanoMsg\Exception as NanoException;

// This should be object
echo gettype (Nano::symbolInfo (0)) . PHP_EOL;

// This should be array
echo gettype (Nano::symbolInfo ()) . PHP_EOL;

// Make sure that all entries have same members
foreach (Nano::symbolInfo () as $sym) {
    if (gettype ($sym->name) != "string" || gettype ($sym->value) != "integer" ||
        gettype ($sym->ns) != "integer"  || gettype ($sym->type) != "integer" ||
        gettype ($sym->unit) != "integer") {
            echo "Fail" . PHP_EOL;
            var_dump ($sym);
    }
}

// Test out of range
try {
    Nano::symbolInfo (500500);
} catch (NanoException $e) {
    echo "ALL GOOD" . PHP_EOL;
}

$test = "1a";
Nano::symbolInfo ($test);
echo gettype ($test) . PHP_EOL;


echo "OK";

--EXPECT--
object
array
ALL GOOD
string
OK