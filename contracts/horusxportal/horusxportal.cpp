/* Title: horusxportal.cpp
*  Description:
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include "horusxportal.hpp"


namespace eosio {

// @abi action
void horusxportal::memberreg( uint64_t member_id, name member_new, const string agreed_terms = "no hash entered" ) {
   require_auth( _self );
   require_auth( member_new );
   require_recipient( member_new );

   eosio_assert( !agreement_terms_hash.compare(agreed_terms) , "terms do not match");

   checksum256 cs{};
   sha256( const_cast<char*>( member_new.to_string().c_str() ), sizeof(member_new), &cs );
   printhex(&cs, sizeof(cs));

   member_table members_portal( _self, _self );

   auto member_account_index = members_portal.get_index<N(accountsha)>();
   auto member_found = member_account_index.find( member::to_key( cs ) );

   if ( member_found != member_account_index.end() ) {
      member_account_index.modify( member_found, 0, [&](auto& m ) {
         m.member_id      = member_id;
         m.agreed_terms   = agreed_terms;
         });
      print("member '", name{member_new}, "' updated\n");
   } else {
      members_portal.emplace( _self /** we pay for RAM **/, [&]( auto& m ) {
         m.member_id      = member_id;
         m.member_sha     = cs;
         m.member_account = member_new;
         m.agreed_terms   = agreed_terms;
      });
      print("member '", name{member_new}, "' registered\n");
    }
}

// @abi action
void horusxportal::memberunreg( name member_leaving ) {
   require_auth( member_leaving );
   require_recipient( member_leaving );

   checksum256 cs{};
   sha256( const_cast<char*>( member_leaving.to_string().c_str() ), sizeof(member_leaving), &cs );
   printhex(&cs, sizeof(cs));

   member_table members_portal( _self, _self );

   auto member_account_index = members_portal.get_index<N(accountsha)>();
   auto member_found = member_account_index.find( member::to_key( cs ) );

   if ( member_found == member_account_index.end() ) {
      string err = "eos account '" + name{member_leaving}.to_string() + "' is not registered";
      eosio_assert( false, err.c_str() );
   }

   member_account_index.erase( member_found );

   print(name{member_leaving}, " successfully unregistered\n");
}

} /// namespace eosio

EOSIO_ABI( eosio::horusxportal, (memberreg)(memberunreg) )
