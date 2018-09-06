# Understanding the EOS.IO RAM Pricing Algorithm
by: gunnar pope

Let's take a deep look at the way block.one implemented their RAM pricing algorithm, which is based upon the Bancor Protocol, to determine the RAM market pricing. The following examples are based off of the `exchange_state.hpp` and `exchange_state.cpp` files found in the `eos/contracts/eosio.system/` folder.


The Bancor algorithm is essentially a smart contract that can provide continuous liquidity between its native Smart Contract token and any other another 'Connector' token. (ToDo: provide more details on the Bancor background). The EOS.IO bancor RAM market is controlled by an equation governed by Bancor algebra which automatically adjusts the price of RAM based upon the supply of native tokens and the current balance of connector tokens.

Let's dive in, starting with the `exchange_state.hpp` file, shown below.

```
#pragma once

#include <eosiolib/asset.hpp>

namespace eosiosystem {
   using eosio::asset;
   using eosio::symbol_type;

   typedef double real_type;

   /**
    *  Uses Bancor math to create a 50/50 relay between two asset types. The state of the
    *  bancor exchange is entirely contained within this struct. There are no external
    *  side effects associated with using this API.
    */
   struct exchange_state {
      asset    supply;

      struct connector {
         asset balance;
         double weight = .5;

         EOSLIB_SERIALIZE( connector, (balance)(weight) )
      };

      connector base;
      connector quote;

      uint64_t primary_key()const { return supply.symbol; }

      asset convert_to_exchange( connector& c, asset in );
      asset convert_from_exchange( connector& c, asset in );
      asset convert( asset from, symbol_type to );

      EOSLIB_SERIALIZE( exchange_state, (supply)(base)(quote) )
   };

   typedef eosio::multi_index<N(rammarket), exchange_state> rammarket;

} /// namespace eosiosystem

```

Starting from the top, the contract imports the `asset` and 'symbol_type' classes from the `eosio` contract.
```
  using eosio::asset;
  using eosio::symbol_type;
```

Next, the variable `real_type` is defined as type `double`, indicating a double precision float that is used for all variables required to execute the Bancor algebra. This insures that each computation is made with high precision and minimal round off error.

```
typedef double real_type;
```


An structure named `exchange_state` is then created that contains all information related to the balance and state of the connector balance.

```
struct exchange_state {
   asset    supply;

   struct connector {
      asset balance;
      double weight = .5;

      EOSLIB_SERIALIZE( connector, (balance)(weight) )
   };
```

This structure holds a few important bits of information. The variable `supply` holds the current supply of native Smart Contract tokens. Within the `exchange_state` structure is the `connector` struct, which holds the balance of connector tokens, CTOK, and the connector weight, `weight`.


Within the `exchange_state` structure, two variable of type `connector` are declared, `base` and `quote`.

```
    connector base;
    connector quote;

```

The variable `base` holds .... and `quote` retains the information of ....

The next section of code is used to get the symbol of the primary currency of the smart contract (??)
```
uint64_t primary_key()const { return supply.symbol; }
```

Finally, three functions are declared to handle the exchanging of assests within the bancor algorithm which all have the return type of `asset`.

```
asset convert_to_exchange( connector& c, asset in );
asset convert_from_exchange( connector& c, asset in );
asset convert( asset from, symbol_type to );
```

The final definition creates a multi-index table that stores the exchange_state and rammarket information needed to record transactions related to the bancor algorithm.


```
typedef eosio::multi_index<N(rammarket), exchange_state> rammarket;
```

### Next step: Look at the exchange_state.cpp to explore how the algorithm works.


## Building the horuspay algorithm

Add a folder to the `eos/contracts/` directory with the same name as your .cpp contract. In this example, we will create a folder named horusbancor/ that contains the horusbancor.cpp and horusbancor.hpp files needed to write the bancor smart contract. We will use these two files to genrate the .abi and .wasm files needed to upload the smart contract to the eosio blockchain by adding a  CMakeLis.txt file to the `horusbancor/` directory within the `eos/contracts/` folder.
