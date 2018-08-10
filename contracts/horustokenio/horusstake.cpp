/* Title:  horustokenstake.cpp
*  Description: Staking implementation is based of the eosio mechanisim for staking resources and voting.
*    See (eos/contacts/eosio.system/delegate_bandwidth.cpp)
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosiolib/transaction.hpp>


namespace horuspaytoken {

   static constexpr time refund_delay = 1; // 3*24*3600;
   static constexpr time refund_expiration_time = 36;//3600;

   /***************************************************************************
    *                               T A B L E S
    *
    * Every user 'from' has a scope/table that uses every receipient 'to'
    * as the primary key.
    *
    *  These tables are designed to be constructed in the scope of the relevant
    *  user, this facilitates simpler API for per-user queries
    **************************************************************************/

   // @abi table userres i64
   struct user_resources {
      account_name  owner;
      asset         horus_weight;
      int64_t       memory_bytes = 0;

      uint64_t primary_key()const { return owner; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( user_resources, (owner)(horus_weight)(memory_bytes) )
   };

   // @abi table stakehorus i64
   struct staked_horus {
      account_name  from;
      account_name  to;
      asset         horus_weight;

      uint64_t  primary_key()const { return to; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( staked_horus, (from)(to)(horus_weight) )

   };

   // @abi table refunds i64
   struct refund_request {
      account_name  owner;
      time          request_time;
      asset         horus_amount;

      uint64_t  primary_key()const { return owner; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( refund_request, (owner)(request_time)(horus_amount) )
   };

   typedef eosio::multi_index< N(userres), user_resources> user_resources_table;
   typedef multi_index< N(stakedhorus), staked_horus>      staked_horus_table;
   typedef multi_index< N(refunds), refund_request>        refunds_table;


   /****************************************************************************
    *                             F U N C T I O N S
    ***************************************************************************/


   void inline horustokenio::delegate_horus( account_name& from, account_name& receiver,
                                              const asset& stake_horus_delta ) {
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
   }


   void inline horustokenio::update_user_resources( account_name& from, account_name& receiver,
                                                    const asset& stake_horus_delta ) {
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
   };


   void inline horustokenio::create_or_update_refund( account_name& from, account_name& receiver, const asset& stake_horus_delta, bool transfer, account_name& source_stake_from )  {
      if ( horuspaytoken::stakingaccount != source_stake_from ) { //for stakingaccount both transfer and refund make no sense
         refunds_table refunds_tbl( _self, from );
         auto req = refunds_tbl.find( from );

         //create/update/delete refund
         auto horus_balance = stake_horus_delta;
         bool need_deferred_trx = false;


         // resources are same sign by assertions in delegatebw and undelegatebw
         bool is_undelegating = (horus_balance.amount ) < 0;
         bool is_delegating_to_self = (!transfer && from == receiver);

         if( is_delegating_to_self || is_undelegating ) {
            if ( req != refunds_tbl.end() ) { //need to update refund
               refunds_tbl.modify( req, 0, [&]( refund_request& r ) {
                  if ( horus_balance < asset(0, HORUS_SYMBOL) ) {
                     r.request_time = now();
                  }
                  r.horus_amount -= horus_balance;
                  if ( r.horus_amount < asset(0, HORUS_SYMBOL) ) {
                     horus_balance = -r.horus_amount;
                     r.horus_amount = asset(0, HORUS_SYMBOL);
                  } else {
                     horus_balance = asset(0, HORUS_SYMBOL);
                  }
               });

               eosio_assert( asset(0, HORUS_SYMBOL) <= req->horus_amount, "negative HORUS refund amount" ); //should never happen

               if ( req->horus_amount == asset(0, HORUS_SYMBOL) ) {
                  refunds_tbl.erase( req );
                  need_deferred_trx = false;
               } else {
                  need_deferred_trx = true;
               }

            } else if ( horus_balance < asset(0, HORUS_SYMBOL) ) { //need to create refund
               refunds_tbl.emplace( from, [&]( refund_request& r ) {
                  r.owner = from;

                  if ( horus_balance < asset(0, HORUS_SYMBOL) ) {
                     r.horus_amount = -horus_balance;
                     horus_balance = asset(0, HORUS_SYMBOL);
                  } // else r.net_amount = 0 by default constructor

                  r.request_time = now();
               });
               need_deferred_trx = true;
            } // else stake increase requested with no existing row in refunds_tbl -> nothing to do with refunds_tbl
         } /// end if is_delegating_to_self || is_undelegating

         if ( need_deferred_trx ) {
            eosio::transaction out;
            out.actions.emplace_back( permission_level{ from, N(active) }, _self, N(refundhorus), from );
            out.delay_sec = refund_delay;
            cancel_deferred( from ); // TODO: Remove this line when replacing deferred trxs is fixed
            out.send( from, from, true );
         } else {
            cancel_deferred( from );
         }

         auto transfer_amount = stake_horus_delta;
         if ( asset(0, HORUS_SYMBOL) < transfer_amount ) {
            INLINE_ACTION_SENDER(horustokenio, transfer)( code, {source_stake_from, N(active)},
               { source_stake_from, horuspaytoken::stakingaccount/* change this to the account accepting the stake*/, asset(transfer_amount), string("staking HORUS") } );
         }
      }
   };


   // based off eos/contacts/eosio.system/delegate_bandwidth.cpp
   void horustokenio::change_resource( account_name from, account_name receiver,
                                       const asset  stake_horus_delta, bool transfer ) {
      require_auth( from );
      eosio_assert( stake_horus_delta != asset(0, HORUS_SYMBOL), "should stake non-zero amount" );

      account_name source_stake_from = from;

      if ( transfer ) {
         from = receiver;
      }

      delegate_horus( from, receiver, stake_horus_delta );
      update_user_resources( from, receiver, stake_horus_delta );
      create_or_update_refund( from, receiver, stake_horus_delta, transfer, source_stake_from );

   }


   /****************************************************************************
    *                             A C T I O N S
    ***************************************************************************/


   void horustokenio::stakehorus( account_name from, account_name receiver, asset stake_horus_quantity, bool transfer ) {
      eosio_assert( stake_horus_quantity >= asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( stake_horus_quantity > asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( !transfer || from != receiver, "cannot use transfer flag if staking to self" );

      change_resource( from, receiver, stake_horus_quantity, transfer);
      print("extra line of code.\n");
   }


   void horustokenio::unstakehorus( account_name from, account_name receiver, asset unstake_horus_quantity ) {
      eosio_assert( asset(0,HORUS_SYMBOL) <= unstake_horus_quantity, "must unstake a positive amount" );

      change_resource( from, receiver, -unstake_horus_quantity, false);
   }


   void horustokenio::refundhorus( const account_name owner ) {
      require_auth( owner );

      refunds_table refunds_tbl( _self, owner );
      auto req = refunds_tbl.find( owner );
      eosio_assert( req != refunds_tbl.end(), "refund request not found" );
      eosio_assert( req->request_time + refund_delay <= now(), "refund is not available yet" );
      // Until now() becomes NOW, the fact that now() is the timestamp of the previous block could in theory
      // allow people to get their tokens earlier than the 3 day delay if the unstake happened immediately after many
      // consecutive missed blocks.

      INLINE_ACTION_SENDER(horuspaytoken::horustokenio, transfer)( code, {horuspaytoken::stakingaccount,N(active)},
                                                    { horuspaytoken::stakingaccount, req->owner, req->horus_amount, string("unstake HORUS") } );

      refunds_tbl.erase( req );
   }

} /// namespace horuspaytoken
