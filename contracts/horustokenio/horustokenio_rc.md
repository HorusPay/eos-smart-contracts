# Action - {{ stakehorus }}

## Description
The intent of the {{ stakehorus }} action is to stake HORUS tokens for access to the HorusPay Payroll Portal database and to mint the digital asset ECASH.

As a HORUS token holder I {{ from }} wish to stake {{ stake_horus_quantity }} to {{ receiver }} granting the {{ receiver }} access to claim staking rewards.  A minimum of 10.0000 HORUS must be staked to participate in staking rewards.  Rewards are earned by staking HORUS tokens for longer than 7 days.  After 7 days payouts can be claimed in the form of ECASH.  ECASH is rewarded proportionally to the amount of HORUS staked and will be issued at a 1% monthly rate of the staked HORUS for a stake greater than or equal to 1 million HORUS, and at a 0.1% monthly rate for any stake less than 1 million.  In the future when the HorusPay Payroll Portal is fully released, the 1 million HORUS token threshold will be removed and all stakes will mint ECASH at a 0.1% monthly rate.  After the 7 day vesting period the staking durration will automatically be renewed for the owner {{ from }} to continue claiming rewards.  This action may change at a later point in time.


# Action - {{ unstakehorus }}

## Description
The intent of the {{ unstakehorus }} action is to unstake existing HORUS tokens.

As a HORUS token staker I {{ from }} wish to unstake {{ stake_id }} for the receiver {{ staked_horus:to }}.  After a 7 day waiting period the HORUS tokens previously staked will become liquid for I {{ staked_horus:from }} to use again.  This action may change at a later point in time.


# Action - {{ claimreward }}

## Description
The intent of the {{ claimreward }} action is allow the {{ staked_horus:owner }} to claim ECASH for a specific {{ staked_horus:id }} that has surpassed the vesting period.  As a staking reward the digital asset ECASH will be issued at a 1% monthly rate of the staked HORUS for a stake greater than or equal to 1 million HORUS and at a 0.1% monthly rate for any stake less than 1 million.  In the future when the HorsuPay Payroll Portal is fully released, the 1 million HORUS token threshold will be removed and all stakes will mint ECASH at a 0.1% monthly rate.  After the 7 day vesting period the staking durration will automatically be renewed for the owner {{ from }} to continue claiming rewards.  This action may change at a later point in time.


# Action - {{ refundhorus }}

## Description
The intent of the {{ refundhorus }} action is to give the {{ staked_horus:owner }} full liquid control of their HORUS tokens after the refund delay has finished.  refundhorus is a delayed transaction that will be called 7 days after unstakehorus is completed.  If for any reason this delayed transaction fails refundhorus can be called manually.  This action may change at a later point in time.
