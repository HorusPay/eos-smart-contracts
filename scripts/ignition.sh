#!/bin/bash

# run this script to begin an eos development session

source setup/setup_env.sh

# quit any running wallet or nodeos
pkill nodeos
pkill keosd

# check operating system
unameOut="$(uname -s)"
case "${unameOut}" in
    Linux*)     machine=Linux;;
    Darwin*)    machine=Mac;;
    CYGWIN*)    machine=Cygwin;;
    MINGW*)     machine=MinGw;;
    *)          machine="UNKNOWN:${unameOut}"
esac
echo ${machine}

echo "Launching nodeos................."

if [ ${machine} =  "Linux" ]; then
   echo "You are running Linux"
   gnome-terminal -e 'nodeos -e -p eosio --delete-all-blocks --plugin eosio::chain_api_plugin --plugin eosio::history_api_plugin'
elif [ ${machine} = "Mac" ]; then
   echo "You are running Mac"
   osascript -e 'tell app "Terminal"
       do script "bash /Volumes/JackDiSalvatoreSSDT5/JacksFiles/EOSIO/HorusPay/eos-smart-contracts/scripts/actions/start_nodeos.sh"
     end tell'
fi

# sleep .75
sleep 5.0

# Open your wallet. (If this is the first time, you may have to create a new default.wallet using the keys provided in setup/setup_env.sh)
echo "---------- Unlocking Default Wallet ... ----------"
bash ${SCRIPTS_HOME}/actions/unlock_wallet.sh

# create some accounts (acct1 and acct2), set in setup/setup_env.sh
#echo "---------- Creating Accounts ... ----------"
#bash ${SCRIPTS_HOME}/actions/accounts.sh


################################ Now, upload some contracts #################
# upload the bios contract and the eosio.token contract to create a new token
echo "---------- Loading the bios contract... ----------"
cleos set contract eosio $EOS_HOME/build/contracts/eosio.bios -p eosio@active

############################### create accounts #################################
# horustokenio
cleos create account eosio ${AUTH_ACCT} ${OWNER_PUBLIC_KEY} ${ACTIVE_PUBLIC_KEY}
# horusbancor
cleos create account eosio ${BANCOR_ACCT} ${OWNER_PUBLIC_KEY} ${ACTIVE_PUBLIC_KEY}

sleep 1.0

echo "---------- Loading the horusbancor contract ------"
cleos set contract horusbancor ${CONTRACTS_HOME}/horus.bancor/build/horusbancor -p horusbancor

echo "---------- Loading the horustokenio contract ------"
cleos set contact horustokenio ${CONTRACTS_HOME}/horus.tokenio/build/horustokenio -p horustokenio

# create a new token
# echo "---------- Creating a new token ... -------------"
bash ${EOS_SCRIPTS_HOME}/actions/create_token.sh

# load the multi-index table example
# cd ${EOS_CONTRACTS_HOME}/youvote
# comptract.sh acct1 youvote

