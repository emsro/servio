#pragma once

#include "../base.hpp"
#include "../cfg/base.hpp"
#include "../iface/base.hpp"
#include "../status.hpp"

#include <span>
#include <string_view>
#include <zll.h>

namespace servio::gvnr
{

struct handle
{
        virtual status on_cmd( iface::cmd_parser cmd ) = 0;

        virtual void current_irq( microseconds now, float current ) = 0;

        virtual void
        metrics_irq( microseconds now, float position, float velocity, bool is_moving ) = 0;

        virtual pwr get_power() const = 0;

        virtual ~handle() = default;
};

struct engage_res
{
        status  stat;
        handle* h;
};

struct governor : zll::ll_base< governor >
{
        virtual cfg::iface&      get_cfg()                               = 0;
        virtual std::string_view name() const                            = 0;
        virtual engage_res       engage( std::span< std::byte > buffer ) = 0;
        virtual status           disengage( handle& )                    = 0;
};

governor* find_governor( std::string_view name );
void      register_governor( governor& gov );

struct governor_autoreg
{
        governor_autoreg( governor& gov )
        {
                register_governor( gov );
        }
};

}  // namespace servio::gvnr
