#!/bin/bash
source setup/setup_env.sh

PASSWORD=$(<${SCRIPTS_HOME}/setup/eosdevkeys.txt)
echo 'Unlocking wallet with local stored password'
cleos wallet unlock --password ${PASSWORD}
