#!/bin/bash

source ~/eos-scripts/setup/bp-endpoints.sh
# input="test_accts.csv"
# input="20180907_accounts_eos.csv"
input="20180907_accounts_eos_trimmed.csv"
while read -r var
do

# cleos -u ${EOSNEWYORK} get table horustokenio $var userres >> staked_horus.txt
cleos -u ${EOSSWEDEN} get table horustokenio $var userres >> staked_horus.txt

done < "$input"
