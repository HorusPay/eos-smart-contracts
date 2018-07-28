/* Title: horustokenio.hpp
*  Description: definitions for the HORUS token contract
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosio.token/eosio.token.hpp>

#define HORUS_SYMBOL S(4,HORUS)
#define HORUS N(HORUS)


namespace horuspaytoken {

using eosio::asset;

static constexpr uint64_t code = N(mytokenaccnt/*change this to real horustokenio account*/);


class horustokenio : public eosio::token {

public:

   explicit horustokenio( account_name self ) : eosio::token( self ) {}

   void stakehorus( account_name from, account_name receiver,
                  asset stake_horus_quantity,
                  bool         transfer );

   void unstakehorus( account_name from, account_name receiver,
                                    asset unstake_horus_quantity );

   void refundhorus( const account_name owner );

private:

   void changebw( account_name from, account_name receiver,
                  const asset   stake_horus_delta,
                  bool          transfer);

/*   struct stake {
      uint64_t id;
      account_name user;
      uint64_t amount;
      uint32_t timestamp;
      uint32_t completion_time;
      bool autorenew = 0;

      auto primary_key()const { return id; }
      account_name get_user()const { return user; }
   };

   typedef eosio::multi_index<N(staketbl), stake,
      indexed_by< N(byuser), const_mem_fun<stake, account_name, &stake::get_user >>
   > staketbl;
*/

};

}
