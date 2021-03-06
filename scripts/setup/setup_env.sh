#!/bin/bash
# setup your eosio development environment

# @author gunnar pope
# modified code from the brilliant work of @scott-schechter, horuspay.io. Thanks scott!

if [ ${USER} = "jackdisalvatore" ]; then
   EOS_HOME="/Volumes/JackDiSalvatoreSSDT5/JacksFiles/EOSIO/eos-v1.2/eos"
fi

# USAGE: Set the path (HORUS_HOME) of all your scripts relative to the Horuspay directory
HORUS_HOME="${HOME}/repos/Horuspay"

if [ ${USER} = "jackdisalvatore" ]; then
   HORUS_HOME="/Volumes/JackDiSalvatoreSSDT5/JacksFiles/EOSIO/HorusPay"
fi

SCRIPTS_HOME="${HORUS_HOME}/eos-smart-contracts/scripts"
CONTRACTS_HOME="${HORUS_HOME}"

## keys used for development ############################################

# the EOSIO system keys are:
EOSIO_PRIVATE_KEY='5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3'
EOSIO_PUBLIC_KEY='EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV'

# the default wallet keys are:
WALLET_PASSWORD='PW5Jw89KejypFTVNLsBg8FnvASPAJhoaJYxAK7d4EU7tLQX6bt7V1'

if [ USER = "jackdisalvatore" ]; then
   WALLET_PASSWORD='PW5JwkoRa4TVJppYJK8gvrbMx89FYF72UbT5d7sD938ys7Uq78dzK'
fi

# the default owner and active keys are
OWNER_PRIVATE_KEY='5KHrknL6tADdwF8JTFSjAjGfFUAKtLiDdSQL24MokRCvkh7Fn9k'
OWNER_PUBLIC_KEY='EOS8K4K68g5zSCsyQLn37TSeRNi3cguWAxZAT2iJdYekeHpMxNszo'
ACTIVE_PRIVATE_KEY='5JDWwPRmAEn7uHAFFGg1q4xrJ5ixjsciH9e3dKG1ArZaSAXSK3p'
ACTIVE_PUBLIC_KEY='EOS7p8h2NZZhtqnuZLRZrnaB5qqLwjwjEMsN95vQY5eLL2ZDr6FBV'

EOSIO_TOKEN="eosio.token"
AUTH_ACCT="horustokenio"
BANCOR_ACCT="horustokenio"
