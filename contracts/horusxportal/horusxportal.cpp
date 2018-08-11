/* Title: horusxportal.cpp
*  Description:
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include "horusxportal.hpp"


namespace horuspayportal {

/***************************************************************************************************
 *                                   F U N C T I O N S
 **************************************************************************************************/

void inline horusxportal::check_resources( name& account_name ) {
   print("Checking Resources\n");
   user_resources_table resources( N(horustokenio), account_name );
   auto user_resource_itr = resources.find( account_name );

   eosio_assert( user_resource_itr->horus_weight >= asset( 10000 , HORUS_SYMBOL ),
                 "Need 1 HORUS staked" );

   print(name{account_name}, " has ", user_resource_itr->horus_weight, "\n");
}

/***************************************************************************************************
 *                                     A C T I O N S
 **************************************************************************************************/

// @abi action
void horusxportal::clientreg( uint64_t         client_id,
                              name             client_account,
                              const string&    client_email,
                              const string&    client_company_name,
                              const string&    client_country,
                              const string&    client_region,
                              const string&    agreed_terms = "no hash provided" ) {
   require_auth( _self );
   require_auth( client_account );
   require_recipient( client_account );

   eosio_assert( !agreement_terms_hash.compare(agreed_terms) , "terms do not match");
   check_resources( client_account );

   checksum256 cs{};
   sha256( const_cast<char*>( client_account.to_string().c_str() ), sizeof(client_account), &cs );

   print("\n**==== DEBUG ====**\n");
   print("client_id: ", client_id, "\n");
   print("client_account: ", name{client_account}, "\n");
   print("client_account sha: ");   printhex(&cs, sizeof(cs)); print("\n");
   print("client_email: ", client_email.c_str(), "\n");
   print("client_company_name: ", client_company_name.c_str(), "\n");
   print("client_country: ", client_country.c_str(), "\n");
   print("client_region: ", client_region.c_str(), "\n");
   print("**===============**\n");

   // Register Client
   client_table clients( _self, _self );

   auto client_account_idx = clients.get_index<N(accountsha)>();
   auto client_itr = client_account_idx.find( to_key( cs ) );

   if ( client_itr != client_account_idx.end() ) {
      // Update exiting client
      client_account_idx.modify( client_itr, 0, [&](auto& c ) {
         c.id                       = client_id;
         c.email                    = client_email;
         c.company_name             = client_company_name;
         c.company_location.country = client_country;
         c.company_location.region  = client_region;
         c.date_modified            = now();
         c.agreed_terms             = agreed_terms;
      });

      print("member '", name{client_account}, "' updated\n");
   } else {
      // Create new client
      clients.emplace( _self /** we pay for RAM **/, [&]( auto& c ) {
         c.id                       = client_id;
         c.account                  = client_account;
         c.account_sha              = cs;
         c.email                    = client_email;
         c.company_name             = client_company_name;
         c.company_location.country = client_country;
         c.company_location.region  = client_region;
         c.date_created             = now();
         c.agreed_terms             = agreed_terms;
      });

      print("member '", name{client_account}, "' registered\n");
   }
}

// @abi action
void horusxportal::clientunreg( name client_leaving ) {
   require_auth( client_leaving );
   require_recipient( client_leaving );

   checksum256 cs{};
   sha256( const_cast<char*>( client_leaving.to_string().c_str() ), sizeof(client_leaving), &cs );

   client_table clients( _self, _self );

   auto client_account_idx = clients.get_index<N(accountsha)>();
   auto client_itr = client_account_idx.find( to_key( cs ) );

   eosio_assert( !(client_itr == client_account_idx.end()) , "client is not registered");

   client_account_idx.erase( client_itr );

   print(name{client_leaving}, " successfully unregistered\n");
}

} /// namespace horuspayportal

EOSIO_ABI( horuspayportal::horusxportal, (clientreg)(clientunreg) )
