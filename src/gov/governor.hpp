#pragma once

#include "../base.hpp"
#include "../cfg/base.hpp"
#include "../iface/base.hpp"
#include "../lib/json_ser.hpp"
#include "../status.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <emlabcpp/pmr/memory_resource.h>
#include <span>
#include <string_view>
#include <zll.h>

namespace servio::gov
{

struct handle
{
        virtual pwr current_irq( microseconds now, float current ) = 0;

        virtual void
        metrics_irq( microseconds now, float position, float velocity, bool is_moving ) = 0;

        virtual ~handle() = default;
};

struct engage_res
{
        status  stat;
        handle* h;

        operator std::tuple< status&, handle*& >()
        {
                return { stat, h };
        }
};

struct governor
{
        virtual cfg::iface*      get_cfg()                                                    = 0;
        virtual std::string_view name() const                                                 = 0;
        virtual status           on_cmd( iface::cmd_parser cmd, servio::iface::root_ser out ) = 0;
        virtual engage_res       engage( em::pmr::memory_resource& mem )                      = 0;
        virtual status           disengage( handle& )                                         = 0;
        virtual ~governor() = default;
};

struct governor_factory : zll::ll_base< governor_factory >
{
        virtual governor* create( em::pmr::memory_resource& ) = 0;
};

void register_factory( governor_factory& factory );
void for_each_factory( em::function_view< void( governor_factory& ) > const& fn );

template < typename T >
struct auto_factory : governor_factory
{
        auto_factory()
        {
                register_factory( *this );
        }

        T* create( em::pmr::memory_resource& resource ) override
        {
                auto* p = resource.allocate( sizeof( T ), alignof( T ) );
                return new ( p ) T();
        }
};

}  // namespace servio::gov
