#!/bin/bash

# Get BUILDDIR and REAL_BITCOIND
CURDIR=$(cd $(dirname "$0"); pwd)
. "${CURDIR}/tests-config.sh"

TESTDIR="$BUILDDIR/qa/tmp/omnicore-rpc-tests"
DATADIR="$TESTDIR/.bitcoin"

# Start clean
rm -rf "$TESTDIR"

git clone https://github.com/OmniLayer/OmniJ.git $TESTDIR
mkdir -p "$DATADIR/regtest"
touch "$DATADIR/regtest/omnicore.log"
cd $TESTDIR
echo "Omni Core RPC test dir: "$TESTDIR
echo "Last OmniJ commit: "$(git log -n 1 --format="%H Author: %cn <%ce>")
if [ "$@" = "true" ]; then
    echo "Debug logging level: minimum"
    $REAL_BITCOIND -regtest -txindex -server -daemon -rpcuser=bitcoinrpc -rpcpassword=pass -debug=0 -omnidebug=none -omnialertallowsender=any -omniactivationallowsender=any -paytxfee=0.0001 -minrelaytxfee=0.00001 -discover=0 -listen=0 -datadir="$DATADIR" &
else
    echo "Debug logging level: maximum"
    $REAL_BITCOIND -regtest -txindex -server -daemon -rpcuser=bitcoinrpc -rpcpassword=pass -debug=1 -omnidebug=all -omnialertallowsender=any -omniactivationallowsender=any -paytxfee=0.0001 -minrelaytxfee=0.00001 -discover=0 -listen=0 -datadir="$DATADIR" &  
fi
$REAL_BITCOINCLI -regtest -rpcuser=bitcoinrpc -rpcpassword=pass -rpcwait getinfo
$REAL_BITCOINCLI -regtest -rpcuser=bitcoinrpc -rpcpassword=pass -rpcwait getinfo_MP
./gradlew --console plain :omnij-rpc:regTest
STATUS=$?
$REAL_BITCOINCLI -regtest -rpcuser=bitcoinrpc -rpcpassword=pass -rpcwait stop

# If $STATUS is not 0, the test failed.
if [ $STATUS -ne 0 ]; then tail -n 200 $DATADIR/regtest/omnicore.log; fi


exit $STATUS
