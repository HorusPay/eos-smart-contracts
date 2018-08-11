# horusxportal contract

This contracts manages the active members of the HorusPay Global Payroll Platform.  There are two different types of members, clients, and vendors.
New clients can join via the `clientreg` action.  A client eos account must have at least 1.0000 HORUS token staked to join.


### Actions

#### clientreg

Registers a new client to the clients database.
Note: You must first stake at least 1.0000 HORUS in the `horustokenio` contract inorder to register.

```bash
$cleos push action horusxportal clientreg '["0","alice","alice@horuspay.com","horuspay","USA","Cali","successhash"]' -p alice horusxportal
```

#### clientunreg

Unregisters client from database.

```bash
$cleos push action horusxportal clientunreg '["alice"]' -p alice
```

### Tables

#### clients

scope: global

```bash
$cleos get table horusxportal horusxportal clients
```


### Data Association

horusxportal Client members MUST have at least 1 HORUS token staked associated with their eos account to join the portal.

