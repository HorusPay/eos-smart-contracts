/* Title: stake_horus.cpp
*  Description: implementation for staking HORUS
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
namespace horuspaytoken {

   using eosio::asset;
   using eosio::multi_index;
   using eosio::indexed_by;
   using eosio::const_mem_fun;
   using eosio::bytes;
   using eosio::print;
   using eosio::permission_level;
   using std::map;
   using std::pair;

   static constexpr time refund_delay = 3*24*3600;
   static constexpr time refund_expiration_time = 3600;

   /*
    * TABLES
    * Every user 'from' has a scope/table that uses every receipient 'to' as the primary key.
    *
    *  These tables are designed to be constructed in the scope of the relevant user, this
    *  facilitates simpler API for per-user queries
    */

   // @abi table userres i64
   struct user_resources {
      account_name  owner;
      asset         horus_weight;
      int64_t       memory_bytes = 0;

      uint64_t primary_key()const { return owner; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( user_resources, (owner)(horus_weight)(memory_bytes) )
   };

   typedef eosio::multi_index< N(userres), user_resources> user_resources_table;

   // @abi table stakehorus i64
   struct staked_horus {
      account_name  from;
      account_name  to;
      asset         horus_weight;

      uint64_t  primary_key()const { return to; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( staked_horus, (from)(to)(horus_weight) )

   };

   typedef multi_index< N(stakedhorus), staked_horus> staked_horus_table;

   // @abi table refunds i64
   struct refund_request {
      account_name  owner;
      time          request_time;
      asset         horus_amount;

      uint64_t  primary_key()const { return owner; }

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( refund_request, (owner)(request_time)(horus_amount) )
   };

   typedef multi_index< N(refunds), refund_request>      refunds_table;
   /*
    * FUNCTIONS
    */

   void horustokenio::changebw( account_name from, account_name receiver,
                                const asset  stake_horus_delta, bool transfer ) {
      require_auth( from );
      eosio_assert( stake_horus_delta != asset(0, HORUS_SYMBOL), "should stake non-zero amount" );

      account_name source_stake_from = from;

      if ( transfer ) {
         from = receiver;
      }


      // update stake delegated from "from" to "receiver"
      {
         staked_horus_table staked_horus( _self, from);

         auto itr = staked_horus.find( receiver );

         if( itr == staked_horus.end() ) {
            itr = staked_horus.emplace( from, [&]( auto& dbo ){
                  dbo.from          = from;
                  dbo.to            = receiver;
                  dbo.horus_weight    = stake_horus_delta;
               });
         }
         else {
            staked_horus.modify( itr, 0, [&]( auto& dbo ){
                  dbo.horus_weight    += stake_horus_delta;
               });
         }

         eosio_assert( asset(0,HORUS_SYMBOL) <= itr->horus_weight, "insufficient staked HORUS" );

         if ( itr->horus_weight == asset(0, HORUS_SYMBOL) ) {
            staked_horus.erase( itr );
         }
      } // itr can be invalid, should go out of scope


      // update totals of "receiver"
      {
         user_resources_table totals_tbl( _self, receiver );

         auto tot_itr = totals_tbl.find( receiver );

         if( tot_itr ==  totals_tbl.end() ) {
            tot_itr = totals_tbl.emplace( from, [&]( auto& tot ) {
                  tot.owner         = receiver;
                  tot.horus_weight    = stake_horus_delta;
               });
         } else {
            totals_tbl.modify( tot_itr, from == receiver ? from : 0, [&]( auto& tot ) {
                  tot.horus_weight    += stake_horus_delta;
               });
         }
         eosio_assert( asset(0, HORUS_SYMBOL) <= tot_itr->horus_weight, "insufficient staked total HORUS" );

         if ( tot_itr->horus_weight == asset(0, HORUS_SYMBOL) && tot_itr->memory_bytes == 0 ) {
            totals_tbl.erase( tot_itr );
         }
      } // tot_itr can be invalid, should go out of scope


      // TODO: Do the actual transfer!
      auto transfer_amount = stake_horus_delta;
      if ( asset(0, HORUS_SYMBOL) < transfer_amount ) {
         INLINE_ACTION_SENDER(eosio::token, transfer)( horuspaytoken::code, {source_stake_from, N(active)},
            { source_stake_from, horuspaytoken::code/* change this to the account accepting the stake*/, asset(transfer_amount), std::string("staking HORUS") } );
      }
   }

   /*
    * ACTIONS
    */

   void horustokenio::stakehorus( account_name from, account_name receiver,
                                asset stake_horus_quantity, bool transfer ) {
      eosio_assert( stake_horus_quantity >= asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( stake_horus_quantity > asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( !transfer || from != receiver, "cannot use transfer flag if staking to self" );

      changebw( from, receiver, stake_horus_quantity, transfer);
      print("extra line of code\n");
   }

   void horustokenio::unstakehorus( account_name from, account_name receiver,
                                  asset unstake_horus_quantity ) {
      eosio_assert( asset(0,HORUS_SYMBOL) <= unstake_horus_quantity, "must unstake a positive amount" );

      changebw( from, receiver, -unstake_horus_quantity, false);
   }

   void horustokenio::refundhorus( const account_name owner ) {
      require_auth( owner );
      print("Not Implemented");
   }

} /// namespace horuspaytoken
