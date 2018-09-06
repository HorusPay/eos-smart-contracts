#!/bin/bash
# @author nsjames

# Use for creating the same accounts every time.
# Great for when you want to blow away an entire and set it back up easily.

# syntax for creating a new account on the mainnet.
# cleos --url https://api.eosnewyork.io:443  system newaccount --stake-net "4000" --stake-cpu "4000" —buy-ram creator newaccount PUB PUB

source setup/setup_env.sh

ACCOUNTS=(acct1 acct2)

create(){

	# but this one does, at least on a local test net
	cleos create account eosio $1 ${OWNER_PUBLIC_KEY}


	# the command below doesn't work for some reason?
	# cleos  system newaccount --stake-net "4000" --stake-cpu "4000" —buy-ram eosio $1 EOS8K4K68g5zSCsyQLn37TSeRNi3cguWAxZAT2iJdYekeHpMxNszo 
}

for name in "${ACCOUNTS[@]}"
do
        create $name
done

