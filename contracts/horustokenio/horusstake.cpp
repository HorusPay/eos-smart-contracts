/* Title:  horustokenstake.cpp
*  Description: Staking implementation is based of the eosio mechanisim for staking resources and voting.
*    See (eos/contacts/eosio.system/delegate_bandwidth.cpp)
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosiolib/transaction.hpp>


namespace horuspaytoken {

   static constexpr time refund_delay     = 7*24*3600;   // 7 days
   const uint64_t REQUIRED_STAKE_DURATION = 7*24*3600;   // 7 days

   /***************************************************************************
    *                               T A B L E S
    *  These tables are designed to be constructed in the scope of the relevant
    *  user, this facilitates simpler API for per-user queries
    **************************************************************************/

   // @abi table userres i64
   struct user_resources {
      account_name  owner;
      asset         total_staked_horus;

      uint64_t primary_key() const { return owner; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( user_resources, (owner)(total_staked_horus) )
   };

   // @abi table stakehorus i64
   struct staked_horus {
      uint64_t      id;
      account_name  from;
      account_name  to;
      asset         horus_weight;
      time          time_initial;

      uint64_t  primary_key() const { return id; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( staked_horus, (id)(from)(to)(horus_weight)(time_initial) )
   };

   // @abi table refunds i64
   struct refund_request {
      account_name  owner;
      time          request_time;
      asset         horus_amount;

      uint64_t  primary_key() const { return owner; }

      // explicit serialization macro is not necessary,
      // used here only to improve compilation time
      EOSLIB_SERIALIZE( refund_request, (owner)(request_time)(horus_amount) )
   };

   typedef multi_index< N(userres), user_resources>       user_resources_table;
   typedef multi_index< N(stakedhorus), staked_horus>       staked_horus_table;
   typedef multi_index< N(refunds), refund_request>              refunds_table;


   /****************************************************************************
    *                             F U N C T I O N S
    ***************************************************************************/


   void inline horustokenio::delegate_horus( account_name& from,
                                             account_name& receiver,
                                             const asset&  stake_horus_delta ) {
      staked_horus_table staked_index( _self, from );

      // emplace a new row for each stake
      auto itr = staked_index.emplace( from /* staker owns RAM */, [&]( auto& stake ){
            stake.id            = staked_index.available_primary_key();
            stake.from          = from;
            stake.to            = receiver;
            stake.horus_weight  = stake_horus_delta;
            stake.time_initial  = now();
         });

      eosio_assert( stake_horus_delta == itr->horus_weight, "staked HORUS failed" );
   }


   void inline horustokenio::update_user_resources( account_name& owner,
                                                    const asset&  stake_horus_delta ) {
      user_resources_table totals_tbl( _self, owner );

      auto tot_itr = totals_tbl.find( owner );

      if( tot_itr ==  totals_tbl.end() ) {
         tot_itr = totals_tbl.emplace( owner, [&]( auto& tot ) {
               tot.owner                 = owner;
               tot.total_staked_horus    = stake_horus_delta;
            });
      } else {
         totals_tbl.modify( tot_itr, 0, [&]( auto& tot ) {
               tot.total_staked_horus    += stake_horus_delta;
            });
      }
      eosio_assert( asset(0, HORUS_SYMBOL) <= tot_itr->total_staked_horus, "insufficient staked total HORUS" );

      if ( tot_itr->total_staked_horus == asset(0, HORUS_SYMBOL) ) {
         totals_tbl.erase( tot_itr );
      }
   };


   void inline horustokenio::create_or_update_refund( account_name& from,
                                                      account_name  receiver,
                                                      const asset&  stake_horus_delta ) {
      refunds_table refunds_tbl( _self, from );
      auto req = refunds_tbl.find( from );

      //create/update/delete refund
      auto horus_balance = stake_horus_delta;
      bool need_deferred_trx = false;

      // resources are same sign by assertions in delegatebw and undelegatebw
      bool is_undelegating = (horus_balance.amount ) < 0;
      bool is_delegating_to_self = ( from == receiver);

      if( is_delegating_to_self || is_undelegating ) {
         if ( req != refunds_tbl.end() ) { //need to update refund
            print("Modifing Refund\n");
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
            print("Creating Refund\n");
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
         print("Creating deferred refund transaction\n");
         eosio::transaction out;
         out.actions.emplace_back( permission_level{ from, N(active) }, _self, N(refundhorus), from );
         out.delay_sec = refund_delay + 1;
         cancel_deferred( from ); // TODO: Remove this line when replacing deferred trxs is fixed
         out.send( from, from, true );
      } else {
         cancel_deferred( from );
      }
   };


   /****************************************************************************
    *                             A C T I O N S
    ***************************************************************************/


   void horustokenio::stakehorus( account_name from, account_name receiver, asset stake_horus_quantity) {
      require_auth( from );
      asset current_staked_horus;

      eosio_assert( is_account( receiver ), "account does not exist");
      eosio_assert( stake_horus_quantity.is_valid(), "invalid offeror_asset");
      eosio_assert( stake_horus_quantity >= asset(0, HORUS_SYMBOL), "must stake a positive amount" );
      eosio_assert( stake_horus_quantity >= asset(100000, HORUS_SYMBOL), "minimum stake required is '10.0000 HORUS'" );

      user_resources_table user_res( _self, from );
      accounts        from_accounts( _self, from );

      auto from_horus_account = from_accounts.find( stake_horus_quantity.symbol.name() );
      auto user_resource_itr        = user_res.find( from );

      if ( user_resource_itr == user_res.end() )
         current_staked_horus = asset(0, HORUS_SYMBOL);
      else
         current_staked_horus = user_resource_itr->total_staked_horus;

      eosio_assert( stake_horus_quantity <= (from_horus_account->balance - current_staked_horus ),
                    "not enough liquid HORUS to stake" );

      delegate_horus( from, receiver, stake_horus_quantity );
      update_user_resources( from, stake_horus_quantity );
   }


   void horustokenio::unstakehorus( account_name from, uint64_t stake_id ) {
      require_auth( from );

      staked_horus_table staked_index( _self, from );

      auto unstake_itr = staked_index.find( stake_id );

      eosio_assert( unstake_itr != staked_index.end(), "staked row does not exist");

      create_or_update_refund( from, unstake_itr->to, -(unstake_itr->horus_weight));

      staked_index.erase( unstake_itr );
   }


   /***********************************************************************************
   *   0.01% per month => ( 0.001 / 30 ) * 7 = 0.00023 = 0.023% per 7 days
   *   Minumum stake:
   *      10.0000 HORUS * 0.00023 = 0.0023 ECASH
   ***********************************************************************************/
   void horustokenio::claimreward( account_name owner, uint64_t stake_id ) {
      require_auth( owner );

      asset    reward;
      int64_t  reward_amount  = 0;
      time     rollover_delta = 0;

      staked_horus_table staked_index( _self, owner );
      auto stake_itr = staked_index.find( stake_id );

      eosio_assert( stake_itr != staked_index.end() , "stake id does not exist" );
      eosio_assert( stake_itr->id == stake_id ,  "failed to retrieved stake id" );

      if ( now() < (stake_itr->time_initial + REQUIRED_STAKE_DURATION) ) {
         string err = "cannot claim reward yet, you still have "
                      + to_string( (stake_itr->time_initial + REQUIRED_STAKE_DURATION) - now() )
                      + " seconds remaining";
        eosio_assert( false, err.c_str() );
      }

      // '10000' is '1.0000 HORUS'         1 Million
      if ( stake_itr->horus_weight >= asset(10000000000, HORUS_SYMBOL) )
      {
          // 1% payouts a month or 0.0023% per week
         reward_amount = int64_t( (stake_itr->horus_weight.amount * 2333) / 1000000 );
      }
      else
      {
          // 0.1% payouts a month or 0.00023% per week
         reward_amount = int64_t( (stake_itr->horus_weight.amount * 2333) / 10000000 );
      }

      reward = asset(reward_amount, ECASH_SYMBOL);
      print("You will get ", reward_amount, "\n");
      eosio_assert( asset(0, ECASH_SYMBOL) < reward , "nothing to be rewarded");   // should never happen

      // Update the staking table time
      rollover_delta = now() - (stake_itr->time_initial + REQUIRED_STAKE_DURATION);
      print("Rollover is ", rollover_delta, " seconds\n");

      staked_index.modify( stake_itr, 0, [&](auto& s) {
         // Subtract on the rollover time that was not accounted for
        s.time_initial = now() - rollover_delta;
      });

      // Sign issue action with both account running 'code' and 'owner'
      vector<permission_level> permissions{ {code,  N(active)} , {owner, N(active)} };

      INLINE_ACTION_SENDER(horustokenio, issue)( code, permissions,
                           { stake_itr->to, reward, string("Rewarding ECASH") } );
   }


   void horustokenio::refundhorus( account_name owner ) {
      require_auth( owner );

      refunds_table refunds_tbl( _self, owner );
      auto req = refunds_tbl.find( owner );
      eosio_assert( req != refunds_tbl.end(), "refund request not found" );
      if ( now() < req->request_time + refund_delay ) {
         string err = "refund is not available yet " + to_string( (req->request_time + refund_delay) - now() )
                     + " seconds remaining";
         eosio_assert( false, err.c_str() );
      }
      // Until now() becomes NOW, the fact that now() is the timestamp of the previous block could in theory
      // allow people to get their tokens earlier than the 3 day delay if the unstake happened immediately after many
      // consecutive missed blocks.

      update_user_resources( owner, -(req->horus_amount) );

      refunds_tbl.erase( req );
   }


} /// namespace horuspaytoken
