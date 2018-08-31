/* Title: change to 'horustokenio_tests.cpp'
*  Description: I am hijacking the eosio.token_tests.cpp file to run unit test for our
*    horustokenio contract.
*  Telegram: @jackdisalvatore
*/
#include <boost/test/unit_test.hpp>
#include <eosio/testing/tester.hpp>
#include <eosio/chain/abi_serializer.hpp>

#include <horustokenio/horustokenio.wast.hpp>
#include <horustokenio/horustokenio.abi.hpp>

#include <Runtime/Runtime.h>

#include <fc/variant_object.hpp>

using namespace eosio::testing;
using namespace eosio;
using namespace eosio::chain;
using namespace eosio::testing;
using namespace fc;
using namespace std;

using mvo = fc::mutable_variant_object;

class horustokenio_tester : public tester {
public:

   horustokenio_tester() {
      produce_blocks( 2 );

      create_accounts( { N(alice), N(bob), N(carol), N(horustokenio) } );
      produce_blocks( 2 );

      set_code( N(horustokenio), horustokenio_wast );
      set_abi( N(horustokenio), horustokenio_abi );

      produce_blocks();

      const auto& accnt = control->db().get<account_object,by_name>( N(horustokenio) );
      abi_def abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser.set_abi(abi, abi_serializer_max_time);
   }

   action_result push_action( const account_name& signer, const action_name &name, const variant_object &data ) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(horustokenio);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary( action_type_name, data, abi_serializer_max_time );

      return base_tester::push_action( std::move(act), uint64_t(signer));
   }

   /*************************************************************************
   *                            G E T T E R S
   **************************************************************************/

   fc::variant get_stats( const string& symbolname )
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(horustokenio), symbol_code, N(stat), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "currency_stats", data, abi_serializer_max_time );
   }

   fc::variant get_account( account_name acc, const string& symbolname)
   {
      auto symb = eosio::chain::symbol::from_string(symbolname);
      auto symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account( N(horustokenio), acc, N(accounts), symbol_code );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "account", data, abi_serializer_max_time );
   }

   fc::variant get_user_resources( account_name owner )
   {
      vector<char> data = get_row_by_account( N(horustokenio), owner, N(userres), owner );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "user_resources", data, abi_serializer_max_time );
   }

   fc::variant get_horus_stake( account_name to, const uint64_t& id )
   {
      vector<char> data = get_row_by_account( N(horustokenio), to, N(stakedhorus), id );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "staked_horus", data, abi_serializer_max_time );
   }

   fc::variant get_refunds( account_name owner )
   {
      vector<char> data = get_row_by_account( N(horustokenio), owner, N(refunds), owner );
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant( "refund_request", data, abi_serializer_max_time );
   }

   /*************************************************************************
   *                            A C T I O N S
   **************************************************************************/

   action_result create( account_name issuer,
                asset        maximum_supply ) {

      return push_action( N(horustokenio), N(create), mvo()
           ( "issuer", issuer)
           ( "maximum_supply", maximum_supply)
      );
   }

   action_result issue( account_name issuer, account_name to, asset quantity, string memo ) {
      return push_action( issuer, N(issue), mvo()
           ( "to", to)
           ( "quantity", quantity)
           ( "memo", memo)
      );
   }

   action_result transfer( account_name from,
                  account_name to,
                  asset        quantity,
                  string       memo ) {
      return push_action( from, N(transfer), mvo()
           ( "from", from)
           ( "to", to)
           ( "quantity", quantity)
           ( "memo", memo)
      );
   }

   action_result stakehorus( account_name from,
                             account_name receiver,
                             asset        stake_horus_quantity,
                             bool         transfer ) {
      return push_action( from, N(stakehorus), mvo()
           ( "from", from)
           ( "receiver", receiver)
           ( "stake_horus_quantity", stake_horus_quantity)
           ( "transfer", transfer)
      );
   }

   action_result unstakehorus( account_name from,
                               uint64_t     stake_id ) {
      return push_action( from, N(unstakehorus), mvo()
           ( "from", from)
           ( "stake_id", stake_id)
      );
   }

   action_result claimreward( account_name owner, uint64_t stake_id ) {
      return push_action( owner, N(claimreward), mvo()
            ( "owner", owner)
            ( "stake_id", stake_id)
      );
   }

   abi_serializer abi_ser;
};


/*************************************************************************
*                            T E S T S
**************************************************************************/

BOOST_AUTO_TEST_SUITE(eosio_token_tests)


/*************************************************************************
* 1) Test creatng a new tokens
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS"));
   auto stats = get_stats("4,HORUS");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0.0000 HORUS")
      ("max_supply", "1200000000.0000 HORUS")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 2) Test creating a token with a negative supply
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_negative_max_supply, horustokenio_tester ) try {

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "max-supply must be positive" ),
      create( N(horustokenio), asset::from_string("-1200000000.0000 HORUS"))
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 3) Test you cannot create two tokens with the same symbol
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( symbol_already_exists, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1200000000 HORUS"));
   auto stats = get_stats("0,HORUS");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0 HORUS")
      ("max_supply", "1200000000 HORUS")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "token with symbol already exists" ),
                        create( N(horustokenio), asset::from_string("100 HORUS"))
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 4) Test max supply of token
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_max_supply, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("4611686018427387903 TKN"));
   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0 TKN")
      ("max_supply", "4611686018427387903 TKN")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

   asset max(10, symbol(SY(0, NKT)));
   share_type amount = 4611686018427387904;
   static_assert(sizeof(share_type) <= sizeof(asset), "asset changed so test is no longer valid");
   static_assert(std::is_trivially_copyable<asset>::value, "asset is not trivially copyable");
   memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

   BOOST_CHECK_EXCEPTION( create( N(horustokenio), max) , asset_type_exception, [](const asset_type_exception& e) {
      return expect_assert_message(e, "magnitude of asset amount must be less than 2^62");
   });

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 5) Test creating token with max decimal places
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( create_max_decimals, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1.000000000000000000 TKN"));
   auto stats = get_stats("18,TKN");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "0.000000000000000000 TKN")
      ("max_supply", "1.000000000000000000 TKN")
      ("issuer", "horustokenio")
   );
   produce_blocks(1);

   asset max(10, symbol(SY(0, NKT)));
   //1.0000000000000000000 => 0x8ac7230489e80000L
   share_type amount = 0x8ac7230489e80000L;
   static_assert(sizeof(share_type) <= sizeof(asset), "asset changed so test is no longer valid");
   static_assert(std::is_trivially_copyable<asset>::value, "asset is not trivially copyable");
   memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

   BOOST_CHECK_EXCEPTION( create( N(horustokenio), max) , asset_type_exception, [](const asset_type_exception& e) {
      return expect_assert_message(e, "magnitude of asset amount must be less than 2^62");
   });

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 6) Test issuing token
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( issue_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS"));
   produce_blocks(1);

   issue( N(horustokenio), N(alice), asset::from_string("1000000000.0000 HORUS"), "HorusPay.io" );

   auto stats = get_stats("4,HORUS");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "1000000000.0000 HORUS")
      ("max_supply", "1200000000.0000 HORUS")
      ("issuer", "horustokenio")
   );

   auto alice_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "1000000000.0000 HORUS")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "quantity exceeds available supply" ),
      issue( N(horustokenio), N(alice), asset::from_string("1000000000.0001 HORUS"), "HorusPay.io" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must issue positive quantity" ),
      issue( N(horustokenio), N(alice), asset::from_string("-1.0000 HORUS"), "HorusPay.io" )
   );

   BOOST_REQUIRE_EQUAL( success(),
      issue( N(horustokenio), N(alice), asset::from_string("1.0000 HORUS"), "HorusPay.io" )
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 7) Test basic token transfering
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( transfer_tests, horustokenio_tester ) try {

   auto token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS"));
   produce_blocks(1);

   issue( N(horustokenio), N(alice), asset::from_string("1000.0000 HORUS"), "HorusPay.io" );

   auto stats = get_stats("4,HORUS");
   REQUIRE_MATCHING_OBJECT( stats, mvo()
      ("supply", "1000.0000 HORUS")
      ("max_supply", "1200000000.0000 HORUS")
      ("issuer", "horustokenio")
   );

   auto alice_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "1000.0000 HORUS")
   );

   transfer( N(alice), N(bob), asset::from_string("300.0000 HORUS"), "HorusPay.io" );

   alice_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_balance, mvo()
      ("balance", "700.0000 HORUS")
   );

   auto bob_balance = get_account(N(bob), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( bob_balance, mvo()
      ("balance", "300.0000 HORUS")
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "overdrawn balance" ),
      transfer( N(alice), N(bob), asset::from_string("701.0000 HORUS"), "HorusPay.io" )
   );

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must transfer positive quantity" ),
      transfer( N(alice), N(bob), asset::from_string("-1000.0000 HORUS"), "HorusPay.io" )
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 8) Test staking HORUS tokens
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( stakehorus_tests, horustokenio_tester ) try {

   auto horus_token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS"));
   produce_blocks(1);

   // Transfer tokens to alice
   issue( N(horustokenio), N(alice), asset::from_string("2000000.0000 HORUS"), "issuing to alice" );

   // minimum balace error
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "minimum stake required is '10.0000 HORUS'" ),
      stakehorus( N(alice), N(alice), asset::from_string("9.9999 HORUS"), false )
   );

   // attempt to stake more HORUS tokens then user has
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "not enough liquid HORUS to stake" ),
      stakehorus( N(alice), N(alice), asset::from_string("2000001.0000 HORUS"), false )
   );

   // stake must be positive amount
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "must stake a positive amount" ),
      stakehorus( N(alice), N(alice), asset::from_string("-100.0000 HORUS"), false )
   );

   // alice stake 1 million HORUS tokens for herself
   // while in Beta 1 million HORUS will mint ECASH at a 0.0023% 7 day rate
   stakehorus( N(alice), N(alice), asset::from_string("1000000.0000 HORUS"), false );
   // alice staking HORUS for bob
   stakehorus( N(alice), N(bob), asset::from_string("100.0000 HORUS"), false );

   auto alice_stake_0 = get_horus_stake( N(alice), 0 );
   REQUIRE_MATCHING_OBJECT( alice_stake_0, mvo()
      ("id", "0")
      ("from", "alice")
      ("to", "alice")
      ("horus_weight", "1000000.0000 HORUS")
      ("time_initial", "1577836804")
   );

   produce_blocks(1);

   auto alice_stake_1 = get_horus_stake( N(alice), 1 );
   REQUIRE_MATCHING_OBJECT( alice_stake_1, mvo()
      ("id", "1")
      ("from", "alice")
      ("to", "bob")
      ("horus_weight", "100.0000 HORUS")
      ("time_initial", "1577836805")
   );

   // Total resources
   auto alice_total_resources = get_user_resources( N(alice) );
   REQUIRE_MATCHING_OBJECT( alice_total_resources, mvo()
      ("owner", "alice")
      ("total_staked_horus", "1000100.0000 HORUS")
   );

   // attempt to transfer more HORUS tokens then user has liquid now that is is staked
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "not enough liquid HORUS to transfer" ),
      transfer( N(alice), N(bob), asset::from_string("1000000.0000 HORUS"), "transfer" )
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 9) Test unstaking HORUS tokens & refunds
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( unstakehorus_tests, horustokenio_tester ) try {

   auto horus_token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS"));
   produce_blocks(1);

   issue( N(horustokenio), N(alice), asset::from_string("2000000.0000 HORUS"), "issuing HORUS" );

   // alice stake 1 million HORUS tokens for herself
   stakehorus( N(alice), N(alice), asset::from_string("100.0000 HORUS"), false );
   // alice staking HORUS for bob
   stakehorus( N(alice), N(bob), asset::from_string("1000000.0000 HORUS"), false );

   produce_blocks(1);

   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "staked row does not exist" ),
       unstakehorus( N(alice), 2 )
   );

   BOOST_REQUIRE_EQUAL( success(),
      unstakehorus( N(alice), 0 )
   );

   auto alice_refunds = get_refunds( N(alice) );
   REQUIRE_MATCHING_OBJECT( alice_refunds, mvo()
      ( "owner", "alice" )
      ( "request_time", "1577836806" )
      ( "horus_amount", "100.0000 HORUS" )
   );

   auto alice_total_resources = get_user_resources( N(alice) );
   REQUIRE_MATCHING_OBJECT( alice_total_resources, mvo()
      ("owner", "alice")
      ("total_staked_horus", "1000100.0000 HORUS")
   );

   // refund stake 0 (100.0000 HORUS) sucessful
   produce_blocks(25);

   alice_total_resources = get_user_resources( N(alice) );
   REQUIRE_MATCHING_OBJECT( alice_total_resources, mvo()
      ("owner", "alice")
      ("total_staked_horus", "1000000.0000 HORUS")
   );

   BOOST_REQUIRE_EQUAL( success(),
      unstakehorus( N(alice), 1 )
   );

   // Make sure that Alice retains ownership of her tokens even when they are staked
   auto alice_HORUS_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_HORUS_balance, mvo()
      ("balance", "2000000.0000 HORUS")
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 10) Test staking HORUS tokens and claiming rewards
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( claimreward_from_tests, horustokenio_tester ) try {

   auto horus_token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS") );
   auto ecash_token = create( N(horustokenio), asset::from_string("1200000000.0000 ECASH") );
   produce_blocks(1);

   issue( N(horustokenio), N(alice), asset::from_string("2000000.0000 HORUS"), "issuing HORUS to alice");

   BOOST_REQUIRE_EQUAL( success(),
      stakehorus( N(alice), N(alice), asset::from_string("1000000.0000 HORUS"), false )
   );
   BOOST_REQUIRE_EQUAL( success(),
      stakehorus( N(alice), N(alice), asset::from_string("1234.0000 HORUS"), false )
   );
   BOOST_REQUIRE_EQUAL( success(),
      stakehorus( N(alice), N(alice), asset::from_string("100.0000 HORUS"), false )
   );

   auto alice_total_resources = get_user_resources( N(alice) );
   REQUIRE_MATCHING_OBJECT( alice_total_resources, mvo()
      ("owner", "alice")
      ("total_staked_horus", "1001334.0000 HORUS")
   );

   produce_blocks(25);

   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 0 )
   );
   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 1 )
   );
   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 2 )
   );
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "stake id does not exist" ),
      claimreward( N(alice), 3 )
   );

   produce_blocks(5);

   auto alice_ECASH_balance = get_account(N(alice), "4,ECASH");
   REQUIRE_MATCHING_OBJECT( alice_ECASH_balance, mvo()
      ("balance", "2333.3111 ECASH")
   );

   produce_blocks(6);

   // claim again
   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 0 )
   );

   alice_ECASH_balance = get_account(N(alice), "4,ECASH");
   REQUIRE_MATCHING_OBJECT( alice_ECASH_balance, mvo()
      ("balance", "4666.3111 ECASH")
   );

   // Make sure that Alice retains ownership of her tokens even when they are staked
   auto alice_HORUS_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_HORUS_balance, mvo()
      ("balance", "2000000.0000 HORUS")
   );

} FC_LOG_AND_RETHROW()


/*************************************************************************
* 11) Test staking HORUS tokens and claiming rewards for someone else
**************************************************************************/
BOOST_FIXTURE_TEST_CASE( claimreward_receiver_tests, horustokenio_tester ) try {

   auto horus_token = create( N(horustokenio), asset::from_string("1200000000.0000 HORUS") );
   auto ecash_token = create( N(horustokenio), asset::from_string("1200000000.0000 ECASH") );
   produce_blocks(1);

   issue( N(horustokenio), N(alice), asset::from_string("2000000.0000 HORUS"), "issuing HORUS to alice");

   BOOST_REQUIRE_EQUAL( success(),
      stakehorus( N(alice), N(bob), asset::from_string("1000000.0000 HORUS"), false )
   );
   BOOST_REQUIRE_EQUAL( success(),
      stakehorus( N(alice), N(bob), asset::from_string("1234.0000 HORUS"), false )
   );
   BOOST_REQUIRE_EQUAL( success(),
      stakehorus( N(alice), N(bob), asset::from_string("100.0000 HORUS"), false )
   );

   auto alice_total_resources = get_user_resources( N(alice) );
   REQUIRE_MATCHING_OBJECT( alice_total_resources, mvo()
      ("owner", "alice")
      ("total_staked_horus", "1001334.0000 HORUS")
   );

   produce_blocks(25);

   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 0 )
   );
   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 1 )
   );
   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 2 )
   );
   BOOST_REQUIRE_EQUAL( wasm_assert_msg( "stake id does not exist" ),
      claimreward( N(alice), 3 )
   );

   produce_blocks(5);

   auto bob_ECASH_balance = get_account(N(bob), "4,ECASH");
   REQUIRE_MATCHING_OBJECT( bob_ECASH_balance, mvo()
      ("balance", "2333.3111 ECASH")
   );

   produce_blocks(6);

   // claim again
   BOOST_REQUIRE_EQUAL( success(),
      claimreward( N(alice), 0 )
   );

   bob_ECASH_balance = get_account(N(bob), "4,ECASH");
   REQUIRE_MATCHING_OBJECT( bob_ECASH_balance, mvo()
      ("balance", "4666.3111 ECASH")
   );

   // Make sure that Alice retains ownership of her tokens even when they are staked
   auto alice_HORUS_balance = get_account(N(alice), "4,HORUS");
   REQUIRE_MATCHING_OBJECT( alice_HORUS_balance, mvo()
      ("balance", "2000000.0000 HORUS")
   );


} FC_LOG_AND_RETHROW()


BOOST_AUTO_TEST_SUITE_END()
