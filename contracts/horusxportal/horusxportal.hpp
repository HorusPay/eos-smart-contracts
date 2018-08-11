/* Title: horusxportal.hpp
*  Description:
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/crypto.h>

#include <string>

#define HORUS_SYMBOL S(4,HORUS)


namespace horuspayportal {

   using eosio::print;
   using eosio::name;
   using eosio::string_to_name;
   using eosio::multi_index;
   using eosio::indexed_by;
   using eosio::const_mem_fun;
   using eosio::permission_level;
   using eosio::asset;
   using std::string;


class horusxportal : public eosio::contract {

public:

   explicit horusxportal( account_name self ) : contract( self ) {}

   void clientreg( uint64_t         client_id,
                   name             client_account,
                   const string&    client_email,
                   const string&    client_company_name,
                   const string&    client_country,
                   const string&    client_region,
                   const string&    agreed_terms );

   void clientunreg( name client_leaving );

private:

   const string agreement_terms_hash{"successhash"};

   static eosio::key256 to_key( const checksum256& account_sha ) {
      const uint64_t* ui64 = reinterpret_cast<const uint64_t*>( &account_sha );
      return eosio::key256::make_from_word_sequence<uint64_t>( ui64[0], ui64[1], ui64[2], ui64[3] );
   }

   void inline check_resources( name& account_name );

   // @abi table clients i64
   struct client {
      uint64_t    id;
      checksum256 account_sha;
      name        account;
      string      email         = "example@company.com";
      string      company_name  = "unregistered";
      time        date_created;
      time        date_modified;
      string      agreed_terms;

      struct location {
        string country;
        string region;
      } company_location = {"USA","Cali"};

      uint64_t primary_key() const { return id; }
      eosio::key256 by_account_sha() const { return to_key( account_sha ); }

      friend eosio::key256 to_key( const checksum256& account_sha );
   };

   // @abi table vendors i64
   struct vendor {
      uint64_t    id;
      checksum256 account_sha;
      name        account;
      /** TODO: Not implemented yet !**/

      uint64_t primary_key() const { return id; }
      eosio::key256 by_account_sha() const { return to_key( account_sha ); }

      friend eosio::key256 to_key( const checksum256& account_sha );
   };

   typedef multi_index<N(clients), client,
      indexed_by<N( accountsha ), const_mem_fun<client, eosio::key256, &client::by_account_sha> >
   > client_table;
   typedef multi_index<N(vendors), vendor,
      indexed_by<N( accountsha ), const_mem_fun<vendor, eosio::key256, &vendor::by_account_sha> >
   > vendor_table;

   /** TAKEN FROM stake_horus.cpp **/
   struct user_resources {
      account_name  owner;
      asset         horus_weight;
      int64_t       memory_bytes = 0;

      uint64_t primary_key()const { return owner; }
   };
   typedef multi_index< N(userres), user_resources> user_resources_table;
   /** TAKEN FROM stake_horus.cpp **/

};

} /// namespace horuspayportal
