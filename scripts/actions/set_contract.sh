#!/bin/bash

# set a contract to a specified account
source ${SCRIPTS_HOME}/setup/setup_env.sh

cleos set contract eosio ${EOS_HOME}/build/contracts/eosio.bios -p eosio@active

cleos create account eosio ${AUTH_ACCT} ${OWNER_PUBLIC_KEY} ${ACTIVE_PUBLIC_KEY}

# executed transaction: 50d6594fd9f81571bc52c7c3a07c8336828304e22c1e52af3b71cae51fad4f35  200 bytes  222 us
#         eosio <= eosio::newaccount            {"creator":"eosio","name":"horusio","owner":{"threshold":1,"keys":[{"key":"EOS8K4K68g5zSCsyQLn37TSeR...
# 2018-09-06T02:36:49.994 thread-0   main.cpp:455                  print_result      warning: transaction executed locally, but may not be confirmed by the network yet
