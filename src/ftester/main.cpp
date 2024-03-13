
#include "ftester/config.hpp"
#include "ftester/flash.hpp"
#include "ftester/system.hpp"

#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

namespace servio::ftester
{

class gtest : public ::testing::Test
{
        std::string                            name_;
        em::testing::test_id                   tid_;
        test_system&                           sys_;
        std::optional< std::filesystem::path > output_dir_;

public:
        gtest(
            test_system&                           sys,
            std::string                            name,
            em::testing::test_id                   tid,
            std::optional< std::filesystem::path > output_dir )
          : name_( name )
          , tid_( tid )
          , sys_( sys )
          , output_dir_( std::move( output_dir ) )
        {
        }

        void SetUp() final
        {
                sys_.clear();
        }

        void TestBody() final
        {
                sys_.run_test( tid_ );
        }

        void TearDown() final
        {

                nlohmann::json j = em::testing::data_tree_to_json( sys_.get_collected() );

                if ( j.contains( "metrics" ) ) {
                        for ( const nlohmann::json& m : j["metrics"] ) {
                                auto k   = m["name"].get< std::string >();
                                auto val = std::to_string( m["value"].get< int >() ) + " " +
                                           m["unit"].get< std::string >();

                                std::cout << std::format( "{:<30} {}", k, val ) << std::endl;
                        }
                }

                if ( !output_dir_.has_value() )
                        return;

                std::filesystem::create_directories( *output_dir_ );
                std::filesystem::path p = output_dir_.value() / ( name_ + ".json" );
                std::ofstream         of{ p };
                of << j.dump();
        }
};

}  // namespace servio::ftester

int main( int argc, char* argv[] )
{
        using namespace std::literals;
        using namespace servio;

        testing::InitGoogleTest( &argc, argv );

        ftester::config cfg = ftester::get_config( argc, argv );

        em::DEBUG_LOGGER.set_option( em::set_stdout( cfg.verbose ) );
        ftester::COM_LOGGER.set_option( em::set_stdout( cfg.verbose ) );
        em::INFO_LOGGER.set_option( em::set_stdout( cfg.verbose ) );

        if ( cfg.firmware.has_value() ) {
                ftester::flash_firmware( cfg.firmware.value(), cfg.openocd_config.value() );
                std::this_thread::sleep_for( 500ms );
        }

        nlohmann::json inpt;
        inpt["powerless"] = cfg.powerless;
        ftester::test_context         ctx{ cfg.d_device, 460800, cfg.c_device, 460800, inpt };
        ftester::controller_interface ctl_iface{ ctx.col_serv };
        ftester::test_system          tsys{ ctx, ctl_iface };

        EMLABCPP_INFO_LOG( "Started initialization" );

        tsys.wait_for_init();

        std::string suite_name = tsys.suite_name();
        for ( auto [tid, name] : tsys.get_tests() ) {
                std::string sname{ name.begin(), name.end() };
                ::testing::RegisterTest(
                    suite_name.c_str(),
                    sname.c_str(),
                    nullptr,
                    nullptr,
                    __FILE__,
                    __LINE__,
                    [&tsys, &cfg, sname, tid] {
                            return new ftester::gtest( tsys, sname, tid, cfg.output_dir );
                    } );
        }

        return RUN_ALL_TESTS();
}
