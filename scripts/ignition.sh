#!/bin/bash

# run this script to begin an eos development session

source setup/setup_env.sh

# quit any running wallet or nodeos
pkill nodeos
pkill keosd

# echo "Launching nodeos................."
gnome-terminal -e 'nodeos -e -p eosio --delete-all-blocks --plugin eosio::chain_api_plugin --plugin eosio::history_api_plugin'
# sleep .75
sleep 5.0

# Open your wallet. (If this is the first time, you may have to create a new default.wallet using the keys provided in setup/setup_env.sh)
echo "---------- Unlocking Default Wallet ... ----------"
bash ${SCRIPTS_HOME}/actions/unlock_wallet.sh 

# create some accounts (acct1 and acct2), set in setup/setup_env.sh
echo "---------- Creating Accounts ... ----------"
bash ${SCRIPTS_HOME}/actions/accounts.sh


################################ Now, upload some contracts #################
# upload the bios contract and the eosio.token contract to create a new token
echo "---------- Loading the bios contract... ----------"
cleos set contract eosio ${EOS_HOME}/build/contracts/eosio.bios -p eosio@active
# bash ${EOS_SCRIPTS_HOME}/actions/set_contract.sh

echo "---------- Loading the horusbancor contract ------"
cleos set contract eosio ${CONTRACTS_HOME}/bancor/horusbancor -p eosio 

# create a new token
# echo "---------- Creating a new token ... -------------"
# bash ${EOS_SCRIPTS_HOME}/actions/create_token.sh

# load the multi-index table example
# cd ${EOS_CONTRACTS_HOME}/youvote
# comptract.sh acct1 youvote

