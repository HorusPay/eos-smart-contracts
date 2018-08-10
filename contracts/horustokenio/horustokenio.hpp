/* Title: horustokenio.hpp
*  Description: definitions for the HORUS token contract
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosio.token/eosio.token.hpp>
#include <string>


#define HORUS_SYMBOL S(4,HORUS)


namespace horuspaytoken {

using eosio::asset;
using eosio::symbol_type;
using eosio::string_to_symbol;
using eosio::multi_index;
using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::bytes;
using eosio::print;
using eosio::permission_level;
using std::string;
using std::map;
using std::pair;

static constexpr uint64_t code = N(horustokenio/*change this to real horustokenio account*/);
static constexpr uint64_t stakingaccount = N(horustokenio/*change this to real horustokenio account*/);


class horustokenio : public eosio::token {

public:

   explicit horustokenio( account_name self ) : eosio::token( self ) {}

   void stakehorus( account_name from,
                    account_name receiver,
                    asset        stake_horus_quantity,
                    bool         transfer );

   void unstakehorus( account_name from,
                      account_name receiver,
                      asset        unstake_horus_quantity );

   void refundhorus( const account_name owner );

   /*void rmtoken( const string sym );*/

private:

   void inline delegate_horus( account_name& from,
                               account_name& receiver,
                               const asset&  stake_horus_delta );

   void inline update_user_resources( account_name& from,
                                      account_name& receiver,
                                      const asset& stake_horus_delta );

   void inline create_or_update_refund( account_name& from,
                                        account_name& receiver,
                                        const asset&  stake_horus_delta,
                                        bool          transfer,
                                        account_name& source_stake_from );

   void change_resource( account_name from,
                         account_name receiver,
                         const asset  stake_horus_delta,
                         bool         transfer );

};

}   // namespace horuspaytoken
