# horustokenio contract

Mainnet account running this contract: `horustokenio`
Token Symbols created: `HORUS`
Issuer account: `horustokenio`
abi:

## How to use this contract

To view account balance

`cleos get currency balance horustokenio account-name-here HORUS`

To transfer tokens (example: 1.0000 HORUS)

```bash
cleos push action horustokenio transfer '["from-account","to-account","1.0000 HORUS","memo"]' -p from-account
```

## Description

This contract allows for the creation of eosio compatible tokens under the class name `horustokenio`.  This is based of the eosio.token contract (https://github.com/EOSIO/eos/blob/master/contracts/eosio.token).

## Actions
### create

Standard currency create action.  Creates a new token asset.

### issue

Standard currency issue action.  Issues tokens to specified accounts.  Requires issuer permission.

### transfer

Standard currency transfer action.  Transfer tokens between two accounts.

---
To build in-source with eos, place the `eosio.token` and `horustokenio` directories into `/eos/contracts` and run `make`

```bash
$cd /eos/build
$make
```

Contact Information

telegram: https://t.me/horuspayUS