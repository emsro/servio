
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
        controller_interface&                  iface;
        test_system&                           sys;
        std::optional< std::filesystem::path > output_dir;

        joque::run_result operator()( const joque::task& )
        {
                joque::run_result res;
                res.retcode = 0;

                sys.clear();

                sys.run_test( tid );

                std::string ss = std::move( iface.errss ).str();
                if ( !ss.empty() ) {
                        joque::record_output( res, joque::output_chunk::ERROR, ss );
                        res.retcode = 1;
                }

                nlohmann::json j = em::testing::data_tree_to_json( sys.get_collected() );

                if ( j.contains( "metrics" ) ) {
                        for ( const nlohmann::json& m : j["metrics"] ) {
                                auto k   = m["name"].get< std::string >();
                                auto val = std::to_string( m["value"].get< int >() ) + " " +
                                           m["unit"].get< std::string >();

                                std::cout << std::format( "{:<20} {:>20}: {}", "", k, val )
                                          << std::endl;
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

        em::DEBUG_LOGGER.set_option( em::set_stdout( cfg.verbose ) );
        ftester::COM_LOGGER.set_option( em::set_stdout( cfg.verbose ) );
        em::INFO_LOGGER.set_option( em::set_stdout( cfg.verbose ) );

        if ( cfg.firmware.has_value() ) {
                ts.tasks["flash"] = ftester::make_flash_task(
                    cfg.firmware.value(), cfg.openocd_config.value(), dev );
                joque::exec( ts ).run();
                ts = joque::task_set{};
                std::this_thread::sleep_for( 500ms );
        }

        nlohmann::json inpt;
        inpt["powerless"] = cfg.powerless;
        ftester::test_context ctx{
            cfg.d_device, cfg.d_baudrate, cfg.c_device, cfg.c_baudrate, inpt };
        ftester::controller_interface ctl_iface{ ctx.col_serv };
        ftester::test_system          tsys{ ctx, ctl_iface };

        EMLABCPP_INFO_LOG( "Started initialization" );

        tsys.wait_for_init();

        std::string suite_name = tsys.suite_name();
        for ( auto [tid, name] : tsys.get_tests() ) {
                std::string sname{ name.begin(), name.size() };
                ts.tasks[sname] = joque::task{
                    .job =
                        ftester::joque_test{
                            .name       = sname,
                            .tid        = tid,
                            .iface      = ctl_iface,
                            .sys        = tsys,
                            .output_dir = cfg.output_dir },
                    .resources = { dev },
                };
        }

        joque::exec_record* rec = joque::exec( ts ).run();
        return rec->failed_count != 0;
}
