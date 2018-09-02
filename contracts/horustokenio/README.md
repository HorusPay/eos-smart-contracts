# horustokenio contract

Mainnet account running this contract: `horustokenio`
Token Symbol: `HORUS`
Token Symbol: `ECASH`

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

This action lets a HORUS token holder stake their tokens for themselves in order to get access to claim staking rewards.
A minimum of 10.0000 HORUS must be staked to participate in staking rewards. Rewards are earned by staking HORUS tokens for
longer than 7 days. After 7 days payouts can be claimed in the form of ECASH. ECASH is rewarded proportionally to the amount of
HORUS staked and will be issued at a 1% monthly rate of the staked HORUS for a stake greater than or equal to 1 million HORUS,
and at a 0.1% monthly rate for any stake less than 1 million. In the future when the HorusPay Payroll Portal is fully released,
the 1 million HORUS token threshold will be removed and all stakes will mint ECASH at a 0.1% monthly rate. After the 7 day vesting
period the staking durration will automatically be renewed for the owner {{ from }} to continue claiming rewards. This action may
change at a later point in time.

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
cleos push action horustokenio unstakehorus '["from","stake_id"]' -p from
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
## Future Updates

### add stake
Add HORUS tokens to an existing stake.  This will restart the initial stake time.

### claimreward - claim all
Claim all available rewards at once.

---
To build in-source with eos, place the `eosio.token` and `horustokenio` directories into `/eos/contracts` and run `make`

```bash
$cd /eos/build
$make
```

Contact Information

telegram: https://t.me/horuspayUS
