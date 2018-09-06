#!/bin/bash

source ${SCRIPTS_HOME}/setup/setup_env.sh

cleos push action ${EOSIO_TOKEN} create '["eosio", "1000000.000 SKI"]' -p ${EOSIO_TOKEN}@active
# cleos push action  create '["eosio", "1000000.000 SKI"]' -p eosio.token@active
