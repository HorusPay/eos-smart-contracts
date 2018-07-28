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

} /// namespace horuspay

EOSIO_ABI( horuspaytoken::horustokenio,(create)(issue)(transfer)(stakehorus)(unstakehorus)(refundhorus))
