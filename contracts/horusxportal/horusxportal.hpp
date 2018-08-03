/* Title: horusxportal.hpp
*  Description:
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include <eosiolib/eosio.hpp>
#include <eosiolib/crypto.h>

#include <string>


namespace eosio {

   using std::string;


class horusxportal : public contract {

public:

   explicit horusxportal( account_name self ) : contract( self ) {}

   void memberreg( uint64_t member_id, name member_new, const string agreed_terms );

   void memberunreg( name member_leaving );

private:

   const string agreement_terms_hash{"successhash"};

   // @abi table members i64
   struct member {
      uint64_t    member_id;
      checksum256 member_sha;     /** hash of account name  **/
      name        member_account; /** eos account name     **/
      string      agreed_terms;   /** hash of agreed terms **/

      uint64_t primary_key() const { return member_id; }
      eosio::key256 by_account_sha() const { return to_key( member_sha ); }

      static eosio::key256 to_key( const checksum256& member_sha ) {
         const uint64_t* ui64 = reinterpret_cast<const uint64_t*>( &member_sha );

         return eosio::key256::make_from_word_sequence<uint64_t>( ui64[0], ui64[1], ui64[2], ui64[3] );
      }
   };

   typedef multi_index<N(members), member,
      indexed_by<N( accountsha ), const_mem_fun<member, eosio::key256, &member::by_account_sha> >
   > member_table;

};

} /// namespace eosio
