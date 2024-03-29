
#include "ftester/config.hpp"
#include "ftester/flash.hpp"
#include "ftester/system.hpp"
#include "joque/exec.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace servio::ftester
{

struct joque_test
{
        std::string                            name;
        em::testing::test_id                   tid;
        test_system&                           sys;
        std::optional< std::filesystem::path > output_dir;

        joque::run_result operator()( const joque::task& )
        {
                joque::run_result res;
                res.retcode = 0;

                em::match(
                    sys.run_test( tid ),
                    [&]( const em::testing::test_result& tres ) {
                            if ( em::testing::is_problematic( tres.status ) )
                                    res.retcode = 2;
                    },
                    [&]( const em::testing::error_variant& err ) {
                            std::stringstream ss;
                            ss << err;
                            joque::record_output( res, joque::output_chunk::ERROR, ss.str() );
                            res.retcode = 1;
                    } );

                nlohmann::json j = em::testing::data_tree_to_json( sys.get_collected() );

                if ( j.contains( "metrics" ) ) {
                        for ( const nlohmann::json& m : j["metrics"] ) {
                                auto k   = m["name"].get< std::string >();
                                auto val = std::to_string( m["value"].get< int >() ) + " " +
                                           m["unit"].get< std::string >();

                                auto msg = std::format( "{:<20} {:>20}: {} \n", "", k, val );
                                joque::record_output( res, joque::output_chunk::STANDARD, msg );
                        }
                }

                if ( !output_dir.has_value() )
                        return res;

                std::filesystem::create_directories( *output_dir );
                std::filesystem::path p = output_dir.value() / ( name + ".json" );
                std::ofstream         of{ p };
                of << j.dump();

                return res;
        }
};

}  // namespace servio::ftester

int main( int argc, char* argv[] )
{
        using namespace std::literals;
        using namespace servio;

        ftester::config cfg = ftester::get_config( argc, argv );
        joque::resource dev;
        joque::task_set ts;

        if ( cfg.verbose ) {
                em::DEBUG_LOGGER.set_option( em::set_stdout( *cfg.verbose ) );
                ftester::COM_LOGGER.set_option( em::set_stdout( *cfg.verbose ) );
                em::INFO_LOGGER.set_option( em::set_stdout( *cfg.verbose ) );
        }

        if ( cfg.output_dir ) {
                static std::ofstream comfile{ cfg.output_dir.value() / "com.log" };
                servio::ftester::COM_LOGGER.set_option( em::set_ostream{ &comfile } );
                static std::ofstream infofile{ cfg.output_dir.value() / "info.log" };
                em::INFO_LOGGER.set_option( em::set_ostream{ &infofile } );
                static std::ofstream debugfile{ cfg.output_dir.value() / "debug.log" };
                em::DEBUG_LOGGER.set_option( em::set_ostream{ &debugfile } );
                static std::ofstream errorfile{ cfg.output_dir.value() / "error.log" };
                em::ERROR_LOGGER.set_option( em::set_ostream{ &errorfile } );
        }

        if ( cfg.firmware.has_value() ) {
                ts.tasks["flash"] = ftester::make_flash_task(
                    cfg.firmware.value(), cfg.openocd_config.value(), dev );
                joque::exec( ts ).run();
                ts = joque::task_set{};
                std::this_thread::sleep_for( 500ms );
        }

        nlohmann::json inpt;
        if ( cfg.input ) {
                std::ifstream f{ *cfg.input };
                f >> inpt;
                inpt = std::move( inpt[0] );
        }
        inpt["powerless"] = cfg.powerless;
        ftester::test_system tsys{
            cfg.d_device, cfg.d_baudrate, cfg.c_device, cfg.c_baudrate, inpt };

        tsys.wait_for_init();

        std::string suite_name = tsys.suite_name();
        for ( auto [tid, name] : tsys.get_tests() ) {
                std::string sname{ name.begin(), name.size() };
                ts.tasks[sname] = joque::task{
                    .job =
                        ftester::joque_test{
                            .name       = sname,
                            .tid        = tid,
                            .sys        = tsys,
                            .output_dir = cfg.output_dir,
                        },
                    .resources = { dev },
                };
        }

        joque::print_exec_visitor           pvis{ false, true };
        std::optional< joque::exec_record > rec = joque::exec( ts, 0, "", pvis ).run();
        if ( rec && cfg.output_dir ) {
                std::ofstream os{ *cfg.output_dir / "res.json" };
                os << nlohmann::json{ *rec }.dump();
        }
        return rec->failed_count != 0;
}
