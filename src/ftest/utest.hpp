#pragma once

#include "../base.hpp"
#include "../gov/governor_manager.hpp"
#include "../status.hpp"

#include <asrtrpp/collect.hpp>
#include <asrtrpp/diag.hpp>
#include <asrtrpp/reac_assm.hpp>
#include <asrtrpp/stream.hpp>
#include <asrtrpp/task_unit.hpp>
#include <emlabcpp/defer.h>
#include <source_location>

namespace em = emlabcpp;

namespace servio::ftest
{

struct utest : asrt::task_test
{
        asrt_reac_assm& assm;
        asrt::flat_id   met_id;

        utest( asrt::task_ctx& ctx, asrt_reac_assm& assm )
          : asrt::task_test( ctx )
          , assm( assm )
        {
        }
};

inline asrt::task< void > init_utest( utest& ctx )
{
        ctx.met_id = co_await asrt::set< asrt::obj >( ctx.assm.collect, 0, "metrics" );
}

asrt::task< void > store_metric( utest& ctx, char const* name, auto&& value, char const* unit = "" )
{
        auto id = co_await asrt::append< asrt::obj >( ctx.assm.collect, ctx.met_id );
        co_await asrt::set( ctx.assm.collect, id, "name", name );
        co_await asrt::set( ctx.assm.collect, id, "unit", unit );
        co_await asrt::set( ctx.assm.collect, id, "value", value );
}

inline asrt::task< void >
expect( utest& ctx, bool condition, std::source_location loc = std::source_location::current() )
{
        if ( !condition ) {
                co_await asrt::rec_diag( ctx.assm.diag, loc.file_name(), loc.line(), nullptr );
                co_yield asrt::with_error{ asrt::test_fail };
        }
}

inline auto setup_poweroff( gov::governor_manager& gv )
{
        return em::defer{ [&] {
                // XXX: maybe improve error handling?
                std::ignore = gv.deactivate();
        } };
}

template < typename T >
void setup_utest( asrt::task_ctx& ctx, asrt_reac_assm& assm, status& res, T def )
{
        if ( res != status::success )
                return;

        using U = asrt::task_unit< T >;
        auto& a = ctx.query( ecor::get_memory_resource );
        void* p = a.allocate( sizeof( U ), alignof( U ) );
        if ( !p ) {
                res = status::error;
                return;
        }

        auto& test = *new ( p ) U{ T{ std::move( def ) } };
        if ( asrt::add_test( assm.reactor, test ) != ASRT_SUCCESS ) {
                res = status::error;
                return;
        }
}

}  // namespace servio::ftest
