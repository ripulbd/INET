#!/bin/sh

# fingerprint tests
cd fingerprint
./runWirelessTests.sh
cd ..

# module tests
cd module
./runWirelessTests.sh
cd ..

# detailed radio tests
cd detailedradio
./runTests.sh
cd ..
