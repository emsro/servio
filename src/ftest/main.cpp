#include "../brd/brd.hpp"
#include "../fw/context.hpp"
#include "../fw/util.hpp"
#include "./setup_tests.hpp"
#include "./testing_system.hpp"

em::pmr::stack_resource< 1024 > TEST_STACK;

int main()
{
        using namespace servio;

        if ( brd::setup_board() != SUCCESS )
                fw::stop_exec();

        core::drivers cdrv = brd::setup_core_drivers();
        if ( cdrv.any_uninitialized() )
                fw::stop_exec();
        fw::context ctx{ std::move( cdrv ) };

        drv::com_iface* dbg_comms = brd::setup_debug_comms();
        if ( dbg_comms == nullptr || dbg_comms->start() != SUCCESS )
                fw::stop_exec();

        ftest::testing_system tsys{ *dbg_comms, "servio tests" };

        status res = SUCCESS;
        ftest::setup_tests(
            TEST_STACK, tsys.reactor, tsys.ctx, tsys.parameters, ctx.cdrv, ctx.core, res );
        if ( res != SUCCESS )
                fw::stop_exec();

        while ( true ) {
                ctx.tick();

                tsys.tick();
        }
}
