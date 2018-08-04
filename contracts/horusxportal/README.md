# horusxportal contract

This contracts manages the active members of the HorusPay Global Payroll Platform.  There are two different types of members, clients, and vendors.
New members can join via the `memberreg` action.


### Member Schema

peusdo javascript data structure schema

```javascript
client = {
   id: Number,                  // Client ID
   account_sha: checksum256,    // Hash of Clients EOS Account
   account: eosio::name,        // Clients EOS Account Name
   company_name: String,        // Clients Company Name
   locaction: {
      country: String,
      province: String,         // Clients Province or State
      city: String
   }
   agreed_terms: String         // Hash to prove Client signed terms of agreement
}
```

```javascript
vendor = {
   id: Number,                  // Vendor ID
   account_sha: checksum256,    // Hash of Vendor EOS Account
   account: eosio::name,        // Vendor EOS Account Name
   company_name: String,        // Vendor Company Name
   locaction: {
      country: String,
      province: String,         // Vendor Province or State
      city: String
   }
   agreed_terms: String         // Hash to prove Vendor signed terms of agreement
}
```

### Data Association

horusxportal Client members MUST have at least 1 HORUS token staked associated with their eos account to join the portal.


### Actions

#### memberreg

For Clients
```bash
$cleos push action horusxportal memberreg '["0","0","alice","Apple","successhash"]' -p alice horusxportal
```

For Vendors
```bash
$cleos push action horusxportal memberreg '["0","1","alice","Apple","successhash"]' -p alice horusxportal
```

#### memberunreg

For Clients
```bash
$cleos push action horusxportal memberunreg '["0","0","alice"]' -p alice
```

For Vendors
```bash
$cleos push action horusxportal memberunreg '["0","1","alice"]' -p alice
```