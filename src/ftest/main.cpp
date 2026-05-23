#include "../brd/brd.hpp"
#include "../fw/context.hpp"
#include "../fw/util.hpp"
#include "./setup_tests.hpp"
#include "./testing_system.hpp"

extern "C" void asrt_log( enum asrt_log_level level, char const* module, char const* fmt, ... )
{
        (void) level;
        (void) module;
        (void) fmt;
}

namespace
{
em::pmr::stack_resource< 1024 > TEST_STACK;
}

int main()
{
        using namespace servio;

        if ( brd::setup_board() != status::success )
                fw::stop_exec();

        core::drivers cdrv = brd::setup_core_drivers();
        if ( cdrv.any_uninitialized() )
                fw::stop_exec();
        fw::context ctx{ cdrv };
        ctx.setup();

        drv::com_iface* dbg_comms = brd::setup_debug_comms();
        if ( dbg_comms == nullptr || dbg_comms->start() != status::success )
                fw::stop_exec();

        ftest::testing_system tsys{ *dbg_comms, "servio tests" };

        status         res = status::success;
        asrt::task_ctx task_ctx{ TEST_STACK };
        ftest::setup_tests( task_ctx, tsys.assm, ctx.cdrv, ctx.core, res );
        if ( res != status::success )
                fw::stop_exec();

        while ( true ) {
                ctx.tick();

                tsys.tick( ctx.cdrv.clock->get_us() );
        }
}
