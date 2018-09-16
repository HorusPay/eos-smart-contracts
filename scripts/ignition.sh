#!/bin/bash
# @author gunnar pope

# modified code from the brilliant work of @scott-schechter, horuspay.io. Thanks scott!
# This file is designed to be a stand alone script so all you need to do is to
# set the paths for the Variable Setup below and it should work.

# Notes:
# 1) This script assumes that you are running the horustokenio contract is
# placed in the ~/eos/contracts/ folder. 

# 2) You may have to move the horusdev.wallet into your ~/eosio-wallet/ folder for the script to run. 
# The keys for the eosio system account and the owner/active keys are imported into that wallet. 

################################# Variable Setup #########################

# USAGE: Set the path (HORUS_HOME) of all your scripts relative to the Horuspay directory
HORUS_HOME="${HOME}/repos/Horuspay"
EOS_CONTRACTS_HOME="${HOME}/eos/contracts"

## keys used for development ############################################

# the EOSIO system keys are:
EOSIO_PRIVATE_KEY='5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
EOSIO_PUBLIC_KEY='EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'

# the default wallet keys are:
# WALLET_PASSWORD='PW5Jw89KejypFTVNLsBg8FnvASPAJhoaJYxAK7d4EU7tLQX6bt7V1'
# the wallet <horusdev.wallet> password
WALLET_PASSWORD='PW5JwVhqEgWWaAqGpj1u6oRwzcq71T3LFPPz7mKSUQuErugN4TGN6'

# the default owner and active keys are
OWNER_PRIVATE_KEY='5KHrknL6tADdwF8JTFSjAjGfFUAKtLiDdSQL24MokRCvkh7Fn9k'
OWNER_PUBLIC_KEY='EOS8K4K68g5zSCsyQLn37TSeRNi3cguWAxZAT2iJdYekeHpMxNszo'
ACTIVE_PRIVATE_KEY='5JDWwPRmAEn7uHAFFGg1q4xrJ5ixjsciH9e3dKG1ArZaSAXSK3p'
ACTIVE_PUBLIC_KEY='EOS7p8h2NZZhtqnuZLRZrnaB5qqLwjwjEMsN95vQY5eLL2ZDr6FBV'

EOSIO_TOKEN="eosio.token"
HORUS_AUTH_ACCT="horustokenio"

TEST_ACCT1="acct1"

################################# Variable Setup #########################

# quit any running wallet or nodeos
pkill nodeos
pkill keosd

# echo "Launching nodeos................."
gnome-terminal -e 'nodeos -e -p eosio --delete-all-blocks --plugin eosio::chain_api_plugin --plugin eosio::history_api_plugin'
# sleep .75
sleep 5.0

# Open your wallet. (If this is the first time, you may have to create a new default.wallet using the keys provided in setup/setup_env.sh)
echo "---------- Unlocking Default Wallet ... ----------"
cleos wallet unlock -n horusdev --password ${WALLET_PASSWORD}

# create some accounts (acct1 and acct2), set in setup/setup_env.sh
echo "---------- Creating Accounts ... ----------"
# create a horustokenio account
cleos create account eosio ${HORUS_AUTH_ACCT} ${OWNER_PUBLIC_KEY}

################################ Now, upload some contracts #################
# upload the bios contract and the eosio.token contract to create a new token
# echo "---------- Loading the bios contract... ----------"
# cleos set contract eosio ${EOS_HOME}/build/contracts/eosio.bios -p eosio@active

echo "---------- Loading the horustokenio contract ------"
# cleos set contract eosio ${EOS_CONTRACTS_HOME}/eosio.token
cleos set contract eosio ${EOS_CONTRACTS_HOME}/horustokenio horustokenio.wasm horustokenio.abi


echo "---------- Create a account for horustokenio contract ------"
cleos create account ${HORUS_AUTH_ACCT} ${TEST_ACCT1} ${OWNER_PUBLIC_KEY}


# create a new token

echo "---------- Issue HORUS to account from horustokenio contract ------"
cleos push action ${HORUS_AUTH_ACCT} issue '["${TEST_ACCT1}", "100.000 HORUS", "memo"]'


# echo "---------- Creating a new token ... -------------"
# bash ${EOS_SCRIPTS_HOME}/actions/create_token.sh

# load the multi-index table example
# cd ${EOS_CONTRACTS_HOME}/youvote
# comptract.sh acct1 youvote
