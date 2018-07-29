/* Title: horustokenio.hpp
*  Description: implementation for the HORUS token contract
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include "horustokenio.hpp"

#include <eosio.token/eosio.token.cpp>
#include "stake_horus.cpp"


namespace horuspaytoken {

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

} /// namespace horuspay

EOSIO_ABI( horuspaytoken::horustokenio,(create)(issue)(transfer)(stakehorus)(unstakehorus)(refundhorus)/*(rmtoken)*/ )
