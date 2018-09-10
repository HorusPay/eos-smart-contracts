#!/bin/bash

input="scott_accounts.csv"
while read -r var
do

cleos -u https://api.eosdetroit.io:443 get table horustokenio $var userres >> staked_horus_scott.txt

done < "$input"
