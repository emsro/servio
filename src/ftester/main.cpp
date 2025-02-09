
#include "../base.hpp"
#include "../ftest/bench/base.hpp"
#include "./config.hpp"
#include "./flash.hpp"
#include "./system.hpp"

#include <emlabcpp/enumerate.h>
#include <fstream>
#include <joque/exec.hpp>
#include <joque/json.hpp>
#include <nlohmann/json.hpp>

namespace servio::ftester
{

bool handle_test_specifics( std::string_view name, test_system& sys, std::filesystem::path out_dir )
{
        if ( name == "current_profile" )  // TODO: I hate this
        {
                auto& recs = sys.get_records();

                std::vector< ftest::bench::prof_record > prof_recs;
                for ( auto const& rec : recs ) {
                        em::protocol::handler< ftest::bench::prof_record >::extract( rec.data )
                            .match(
                                [&]( ftest::bench::prof_record const& pr ) {
                                        prof_recs.push_back( pr );
                                },
                                [&]( auto&& err ) {
                                        EMLABCPP_ERROR_LOG( "Failed to parse record: ", err );
                                } );
                }

                std::ofstream of{ out_dir / "current_profile.csv" };
                for ( auto&& [i, prec] : em::enumerate( prof_recs ) ) {
                        if ( prec.count == 0 )
                                continue;
                        auto avg = prec.sum | std::views::transform( [&]( uint32_t val ) {
                                           return std::ceil(
                                               static_cast< float >( val ) /
                                               static_cast< float >( prec.count ) );
                                   } );
                        of << em::view{ avg } << "\n";
                }

                return true;
        }

        return false;
}

struct joque_test
{
        std::string                  name;
        em::testing::test_id         tid;
        test_system&                 sys;
        opt< std::filesystem::path > output_dir;

        joque::run_result operator()( joque::task const& )
        {
                joque::run_result res;
                res.retcode = 0;

                em::match(
                    sys.run_test( tid ),
                    [&]( em::testing::test_result const& tres ) {
                            if ( em::testing::is_problematic( tres.status ) )
                                    res.retcode = 2;
                    },
                    [&]( em::testing::error_variant const& err ) {
                            std::stringstream ss;
                            ss << err;
                            joque::insert_err( res, ss.str() );
                            res.retcode = 1;
                    } );

                nlohmann::json j = em::testing::data_tree_to_json( sys.get_collected() );

                if ( res.retcode != 0 ) {
                        std::string s = j.dump( 2 );
                        if ( s.length() > 2048 )
                                s = "output json too big, not shown";
                        joque::insert_err( res, s );
                } else if ( j.contains( "metrics" ) ) {
                        for ( nlohmann::json const& m : j["metrics"] ) {
                                auto k   = m["name"].get< std::string >();
                                auto val = std::to_string( m["value"].get< int >() ) + " " +
                                           m["unit"].get< std::string >();

                                auto msg = std::format( "{:<20} {:>20}: {} \n", "", k, val );
                                joque::insert_std( res, msg );
                        }
                }

                if ( !output_dir.has_value() )
                        return res;

                std::filesystem::create_directories( *output_dir );
                std::filesystem::path p = output_dir.value() / ( name + ".json" );
                std::ofstream         of{ p };
                of << j.dump();

                bool specific_test = handle_test_specifics( name, sys, *output_dir );

                auto& recs = sys.get_records();
                if ( !recs.empty() && !specific_test ) {
                        std::filesystem::path recp = output_dir.value() / ( name + ".rec" );
                        std::ofstream         of{ recp };
                        for ( auto const& rec : recs )
                                of << em::convert_view< int >( rec.data ) << std::endl;
                }

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
                    cfg.firmware.value(), cfg.openocd_config.value(), std::nullopt, dev );
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

        joque::print_exec_visitor pvis{ false };
        opt< joque::exec_record > rec = joque::exec( ts, 0, cfg.filter, pvis ).run();
        if ( rec && cfg.output_dir ) {
                std::ofstream os{ *cfg.output_dir / "res.json" };
                os << nlohmann::json{ *rec }.dump();
        }
        return rec->stats[joque::run_status::FAIL] != 0;
}
