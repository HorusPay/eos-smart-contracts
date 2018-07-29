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
cleos push action horustokenio transfer '["from","to","1.0000 HORUS","memo"]' -p from
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

### stakehorus

Stake your tokens for yourself or another account.

```bash
cleos push action horustokenio stakehorus '["from","receiver","1.0000 HORUS"]' -p from
```

### unstakehorus

Unstake tokens back to the staker

```bash
cleos push action horustokenio unstakehorus '["from","receiver","1.0000 HORUS"]' -p from
```

### refundhorus

Called when unstaking to refund the staked tokens over a period of time.

---
To build in-source with eos, place the `eosio.token` and `horustokenio` directories into `/eos/contracts` and run `make`

```bash
$cd /eos/build
$make
```

Contact Information

telegram: https://t.me/horuspayUS