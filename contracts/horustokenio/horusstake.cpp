/* Title:  horustokenstake.cpp
*  Description: Staking implementation is based of the eosio mechanisim for staking resources and voting.
*    See (eos/contacts/eosio.system/delegate_bandwidth.cpp)
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosiolib/transaction.hpp>


namespace horuspaytoken {

   static constexpr time refund_delay = 7*24*3600;   // 7 days
   const uint64_t REQUIRED_STAKE_DURATION = 5*24*3600; // 5 days

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
      uint64_t      id;
      account_name  from;
      account_name  to;
      asset         horus_weight;
      time          time_initial;

      uint64_t  primary_key()const { return id; }
      uint64_t  get_staker()const { return to; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( staked_horus, (id)(from)(to)(horus_weight)(time_initial) )
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

   typedef multi_index< N(userres), user_resources> user_resources_table;
   typedef multi_index< N(refunds), refund_request>        refunds_table;
   typedef multi_index< N(stakedhorus), staked_horus,
            indexed_by< N(bystaker), const_mem_fun<staked_horus, account_name, &staked_horus::get_staker >>
            >      staked_horus_table;


   /****************************************************************************
    *                             F U N C T I O N S
    ***************************************************************************/


   void inline horustokenio::delegate_horus( account_name& from,
                                             account_name& receiver,
                                             const asset&  stake_horus_delta ) {
      staked_horus_table staked_index( _self, from );

      // emplace a new row for each stake
      auto itr = staked_index.emplace( from /* staker owns RAM */, [&]( auto& dbo ){
            dbo.id            = staked_index.available_primary_key();
            dbo.from          = from;
            dbo.to            = receiver;
            dbo.horus_weight  = stake_horus_delta;
            dbo.time_initial  = now();
         });

      eosio_assert( asset(0,HORUS_SYMBOL) <= itr->horus_weight, "insufficient staked HORUS" );

   }


   void inline horustokenio::update_user_resources( account_name& from,
                                                    account_name& receiver,
                                                    const asset&  stake_horus_delta ) {
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


   void inline horustokenio::create_or_update_refund( account_name& from,
                                                      account_name& receiver,
                                                      const asset&  stake_horus_delta,
                                                      bool          transfer,
                                                      account_name& source_stake_from )  {
      //for stakingaccount both transfer and refund make no sense
      if ( horuspaytoken::stakingaccount != source_stake_from ) {
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

      }
   };


   /****************************************************************************
    *                             A C T I O N S
    ***************************************************************************/


   void horustokenio::stakehorus( account_name from, account_name receiver, asset stake_horus_quantity, bool transfer ) {
      eosio_assert( stake_horus_quantity >= asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( stake_horus_quantity > asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( !transfer || from != receiver, "cannot use transfer flag if staking to self" );

      require_auth( from );
      eosio_assert( stake_horus_quantity >= asset(1000, HORUS_SYMBOL), "minimum stake required is '0.1000 HORUS'" );

      account_name source_stake_from = from;

      if ( transfer ) {
         from = receiver;
      }

      delegate_horus( from, receiver, stake_horus_quantity );
      //update_user_resources( from, receiver, stake_horus_delta );
      create_or_update_refund( from, receiver, stake_horus_quantity, transfer, source_stake_from );

      auto transfer_amount = stake_horus_quantity;
      if ( asset(0, HORUS_SYMBOL) < transfer_amount ) {
         INLINE_ACTION_SENDER(horustokenio, transfer)( code, {source_stake_from, N(active)},
            { source_stake_from, stakingaccount,
                                 asset(transfer_amount), string("staking HORUS") } );
      }
   }


   void horustokenio::unstakehorus( account_name from, uint64_t unstake_id ) {
      require_auth( from );

      account_name source_stake_from = from;

      staked_horus_table staked_index( _self, from );

      auto unstake_itr = staked_index.find( unstake_id );

      eosio_assert( unstake_itr != staked_index.end(), "staked row does not exist!");

      create_or_update_refund( from, from, -(unstake_itr->horus_weight), true, source_stake_from );

      staked_index.erase( unstake_itr );

   }


   void horustokenio::claimreward( account_name owner, uint64_t stake_id ) {
      require_auth( owner );
      asset reward;
      uint64_t reward_amount = 0 ;
      time rollover_delta = 0;

      staked_horus_table staked_index( _self, owner );
      auto stake_itr = staked_index.find( stake_id );

      eosio_assert( stake_itr != staked_index.end() , "stake id does not exist" );

      if ( now() <= (stake_itr->time_initial + REQUIRED_STAKE_DURATION) ) {
         string err = "cannot claim reward yet, you still have "
                      + to_string( (stake_itr->time_initial + REQUIRED_STAKE_DURATION) - now() )
                      + " seconds remaining";
         eosio_assert( false, err.c_str() );
      }

      // '10000' is '1.0000 HORUS'         1 Million
      if ( stake_itr->horus_weight > asset(10000000000, HORUS_SYMBOL) )
      {
         reward_amount = (stake_itr->horus_weight.amount / 100) * 1 ; // 1%
      }
      else if ( stake_itr->horus_weight < asset(1000000, HORUS_SYMBOL) )
      {
         reward_amount = (stake_itr->horus_weight.amount / 1000) * 1 ; // 0.1%
      }

      reward = asset(reward_amount, ECASH_SYMBOL);
      eosio_assert( asset(0, ECASH_SYMBOL) < reward , "nothing to be rewarded");
      print("You will get ", reward, "\n");

      /* Update the staking table time */
      rollover_delta = now() - (stake_itr->time_initial + REQUIRED_STAKE_DURATION);
      print("Roller is ", rollover_delta, " seconds\n");
      staked_index.modify( stake_itr, 0, [&](auto& s) {
         // Subtract on the rollover time that was not accounted for
         s.time_initial = now() - rollover_delta;
      });

      INLINE_ACTION_SENDER(horustokenio, issue)( code, {stakingaccount,N(active)},
                           { stake_itr->to, reward, string("Rewarding ECASH") } );
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

      INLINE_ACTION_SENDER(horustokenio, transfer)( code, {stakingaccount,N(active)},
                           { stakingaccount, req->owner, req->horus_amount, string("unstake HORUS") } );

      refunds_tbl.erase( req );
   }


} /// namespace horuspaytoken
