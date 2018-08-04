/* Title: horusxportal.hpp
*  Description:
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosiolib/eosio.hpp>
#include <eosiolib/crypto.h>


#include <string>


namespace horuspay {

   using eosio::print;
   using eosio::name;
   using eosio::string_to_name;
   using eosio::multi_index;
   using eosio::indexed_by;
   using eosio::const_mem_fun;
   using eosio::permission_level;

   using std::string;


class horusxportal : public eosio::contract {

public:

   explicit horusxportal( account_name self ) : contract( self ) {}

   enum member_types {
      CLIENT =  0,
      VENDOR /* 1 */
   };

   void memberreg( uint64_t         member_id,
                   uint32_t         member_type,
                   name             member_new,
                   const string&    company_name,
                   const string&    agreed_terms );

   void memberunreg( name member_leaving, uint32_t member_type );

private:

   const string agreement_terms_hash{"successhash"};

   static eosio::key256 to_key( const checksum256& account_sha ) {
      const uint64_t* ui64 = reinterpret_cast<const uint64_t*>( &account_sha );
      return eosio::key256::make_from_word_sequence<uint64_t>( ui64[0], ui64[1], ui64[2], ui64[3] );
   }

   // @abi table clients i64
   struct client {
      uint64_t    id;
      checksum256 account_sha;
      name        account;
      string      company_name = "Unregistered";
      string      agreed_terms;

      uint64_t primary_key() const { return id; }
      eosio::key256 by_account_sha() const { return to_key( account_sha ); }

      friend eosio::key256 to_key( const checksum256& account_sha );
   };

   // @abi table vendors i64
   struct vendor {
      uint64_t    id;
      checksum256 account_sha;
      name        account;
      string      company_name = "Unregistered";
      string      agreed_terms;

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


};

} /// namespace horuspay
