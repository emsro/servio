#include "brd/brd.hpp"
#include "ftest/setup_tests.hpp"
#include "ftest/testing_system.hpp"
#include "fw/context.hpp"

em::pmr::stack_resource< 1024 > TEST_STACK;

int main()
{
        using namespace servio;

        if ( brd::setup_board() != em::SUCCESS )
                fw::stop_exec();

        fw::context ctx = fw::setup_context();

        drv::com_iface* dbg_comms = brd::setup_debug_comms();
        if ( dbg_comms == nullptr || dbg_comms->start() != em::SUCCESS )
                fw::stop_exec();

        ftest::testing_system tsys{ *dbg_comms, "basic tests" };

        em::result res = em::SUCCESS;
        ftest::setup_tests(
            TEST_STACK, tsys.reactor, tsys.ctx, tsys.parameters, ctx.cdrv, ctx.core, res );
        if ( res != em::SUCCESS )
                fw::stop_exec();

        while ( true ) {
                ctx.tick();

                tsys.tick();
        }
}
