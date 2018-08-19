# horustokenio contract

Mainnet account running this contract: `horustokenio`
Token Symbol: `HORUS`

Jungle Testnet account running this contract: `horustokenio`
Token Symbol: `HORUS`
Token Symbol: `ECASH`

## How to use this contract

To view account balance

`cleos get currency balance horustokenio account-name HORUS`

`cleos get currency balance horustokenio account-name ECASH`

To transfer tokens (example: 1.0000 HORUS)

```bash
cleos push action horustokenio transfer '["from","to","1.0000 HORUS","memo"]' -p from

cleos push action horustokenio transfer '["from","to","1.0000 ECASH","memo"]' -p from
```

## Description

This contract allows for the creation of eosio compatible tokens under the class name `horustokenio`.  This is based off the eosio.token contract (https://github.com/EOSIO/eos/blob/master/contracts/eosio.token).

## Actions
### create

Standard currency create action.  Creates a new token asset.

### issue

Standard currency issue action.  Issues tokens to specified accounts.  Requires issuer permission.

### transfer

Standard currency transfer action.  Transfer tokens between two accounts.

### stakehorus

Stake your HORUS tokens for yourself or another account.  Staking works by sending HORUS tokens to the `horustokenio` contract account via
the `stakehorus` action.  Each time tokens are staked a new `stakedhorus` row is created.  Each row is indexed by a `stake_id`.
Each stake row is timestamped inorder to determine if rewards can be claimed.  When rewards are ready to be claimed, the `claimreward`
action can be called with the `stake_id`.  If the stake exceeds 1000000.0000 HORUS a payout rate of 1% is issued every 5 days,
otherwise a payout rate of 0.1% is issued every 5 days.  Payouts are issued in ECASH.

```bash
cleos push action horustokenio stakehorus '["from","receiver","1.0000 HORUS"]' -p from
```
ex:
```bash
cleos push action horustokenio stakehorus '["alice","alice","1.0000 HORUS"]' -p alice

cleos get table horustokenio alice stakedhorus
{
  "rows": [{
      "id": 0,
      "from": "alice",
      "to": "alice",
      "horus_weight": "5.0000 HORUS",
      "time_initial": 1534699728
    }
  ],
  "more": false
}
```

### unstakehorus

Unstake tokens back to the staker.  There is a 7 day delay to unstake.

```bash
cleos push action horustokenio unstakehorus '["from","receiver","stake_id"]' -p from
```

### claimreward

```bash
cleos push action horustokenio claimreward '["owner","stake_id"]' -p owner
```

### refundhorus

Called when unstaking to refund the staked tokens over a period of time.

```bash
cleos get table horustokenio owner refunds
```

---
To build in-source with eos, place the `eosio.token` and `horustokenio` directories into `/eos/contracts` and run `make`

```bash
$cd /eos/build
$make
```

Contact Information

telegram: https://t.me/horuspayUS
