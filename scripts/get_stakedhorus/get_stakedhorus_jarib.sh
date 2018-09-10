#!/bin/bash

input="jarib_accounts.csv"
while read -r var
do

cleos -u https://api.eosnewyork.io:443 get table horustokenio $var userres >> staked_horus_jarib.txt

done < "$input"
