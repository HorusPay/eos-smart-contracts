#!/bin/bash

input="jack_accounts.csv"
while read -r var
do

cleos -u https://node2.eosphere.io get table horustokenio $var userres >> staked_horus_jack.txt

done < "$input"
