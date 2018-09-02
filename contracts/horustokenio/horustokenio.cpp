/* Title: horustokenio.hpp
*  Description: implementation for the HORUS token contract
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include "horustokenio.hpp"

#include <eosio.token/eosio.token.cpp>
#include "horusstake.cpp"


namespace horuspaytoken {

   void horustokenio::transfer( account_name from,
                                account_name to,
                                asset        quantity,
                                string       memo ) {
      eosio_assert( from != to, "cannot transfer to self" );
      require_auth( from );
      eosio_assert( is_account( to ), "to account does not exist");
      auto sym = quantity.symbol.name();
      stats statstable( _self, sym );
      const auto& st = statstable.get( sym );

      require_recipient( from );
      require_recipient( to );

      eosio_assert( quantity.is_valid(), "invalid quantity" );
      eosio_assert( quantity.amount > 0, "must transfer positive quantity" );
      eosio_assert( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );
      eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

      auto payer = has_auth( to ) ? to : from;

      if ( quantity.symbol == HORUS_SYMBOL ) {
         /* Check staked horus in user resources */
         user_resources_table user_res( _self, from );
         auto user_resource_itr = user_res.find( from );

         if ( user_resource_itr != user_res.end() && user_resource_itr->total_staked_horus.amount ) {
            accounts from_acnts( _self, from );

            const auto& owner = from_acnts.get( quantity.symbol.name(), "no HORUS balance object found" );
            eosio_assert(quantity <= (owner.balance - user_resource_itr->total_staked_horus),
                         "not enough liquid HORUS to transfer");
         }
      }

      sub_balance( from, quantity );
      add_balance( to, quantity, payer );
   }

// IMPLEMENT FUTURE ACTIONS HERE

   /* DELETE THIS: Only for testing */
/*   void horustokenio::rmtoken( const string sym ) {
      symbol_type symb = string_to_symbol(4, sym.c_str());
      print("Symbol is ", sym, "\n");
      print( symb.name() );

      token::stats statstable( _self, symb.name() );
      const auto& st = statstable.get( symb.name() );
      print("\nCurrent supply is ", st.supply);
      statstable.erase(st);
   }*/

} /// namespace horuspaytoken

EOSIO_ABI( horuspaytoken::horustokenio,(create)(issue)(transfer)(stakehorus)(unstakehorus)(refundhorus)(claimreward)/*(rmtoken)*/ )
