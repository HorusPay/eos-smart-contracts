/* Title: horusxportal.cpp
*  Description:
*  Author: Jack DiSalvatore
*  Telegram: @jackdisalvatore
*/
#include "horusxportal.hpp"


namespace horuspay {

// @abi action
void horusxportal::memberreg( uint64_t         member_id,
                              uint32_t         member_type,
                              name             member_new,
                              const string&    company_name = "Unregistered",
                              const string&    agreed_terms = "no hash entered" ) {
   require_auth( _self );
   require_auth( member_new );
   require_recipient( member_new );

   eosio_assert( !agreement_terms_hash.compare(agreed_terms) , "terms do not match");
   eosio_assert( member_type == CLIENT || member_type == VENDOR, "invalid member type");

   checksum256 cs{};
   sha256( const_cast<char*>( member_new.to_string().c_str() ), sizeof(member_new), &cs );
   printhex(&cs, sizeof(cs));

   print("\n**==== DEBUG ====**\n");
   print("Member ID: ", member_id, "\n");
   print("Member Type: ", member_type, "\n");
   print("member_account: ", name{member_new}, "\n");
   print("company: ", company_name.c_str(), "\n");

   // Register Client
   if ( member_type == CLIENT ) {
      client_table client_members( _self, _self );

      auto members_account_index = client_members.get_index<N(accountsha)>();
      auto member_itr = members_account_index.find( to_key( cs ) );

      if ( member_itr != members_account_index.end() ) {
         members_account_index.modify( member_itr, 0, [&](auto& m ) {
            m.id           = member_id;
            m.company_name = company_name;
            m.agreed_terms = agreed_terms;
            });

         print("member '", name{member_new}, "' updated\n");
      } else {
         client_members.emplace( _self /** we pay for RAM **/, [&]( auto& m ) {
            m.id           = member_id;
            m.account_sha  = cs;
            m.account      = member_new;
            m.company_name = company_name;
            m.agreed_terms = agreed_terms;
         });
      }
   }

   // Register Vendor
   if ( member_type == VENDOR ) {
      vendor_table vendor_members( _self, _self );

      auto members_account_index = vendor_members.get_index<N(accountsha)>();
      auto member_itr = members_account_index.find( to_key( cs ) );

      if ( member_itr != members_account_index.end() ) {
         members_account_index.modify( member_itr, 0, [&](auto& m ) {
            m.id           = member_id;
            m.company_name = company_name;
            m.agreed_terms = agreed_terms;
            });

         print("member '", name{member_new}, "' updated\n");
      } else {
         vendor_members.emplace( _self /** we pay for RAM **/, [&]( auto& m ) {
            m.id           = member_id;
            m.account_sha  = cs;
            m.account      = member_new;
            m.company_name = company_name;
            m.agreed_terms = agreed_terms;
         });
      }
   }


   print("member '", name{member_new}, "' registered\n");

}

// @abi action
void horusxportal::memberunreg( name member_leaving, uint32_t member_type ) {
   require_auth( member_leaving );
   require_recipient( member_leaving );

   checksum256 cs{};
   sha256( const_cast<char*>( member_leaving.to_string().c_str() ), sizeof(member_leaving), &cs );
   printhex(&cs, sizeof(cs));

   // Remove Client
   if ( member_type == CLIENT ) {
      client_table client_members( _self, _self );

      auto members_account_index = client_members.get_index<N(accountsha)>();
      auto member_itr = members_account_index.find( to_key( cs ) );

      if ( member_itr == members_account_index.end() ) {
         string err = "eos account '" + name{member_leaving}.to_string() + "' is not registered";
         eosio_assert( false, err.c_str() );
      }

      members_account_index.erase( member_itr );
   }

   // Remove Vendor
   if ( member_type == VENDOR ) {
      vendor_table vendor_members( _self, _self );

      auto members_account_index = vendor_members.get_index<N(accountsha)>();
      auto member_itr = members_account_index.find( to_key( cs ) );

      if ( member_itr == members_account_index.end() ) {
         string err = "eos account '" + name{member_leaving}.to_string() + "' is not registered";
         eosio_assert( false, err.c_str() );
      }

      members_account_index.erase( member_itr );
   }

   print(name{member_leaving}, " successfully unregistered\n");
}

} /// namespace horuspay

EOSIO_ABI( horuspay::horusxportal, (memberreg)(memberunreg) )
