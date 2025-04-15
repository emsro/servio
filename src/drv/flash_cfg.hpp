#pragma once

#include "../plt/platform.hpp"
#include "./interfaces.hpp"

#include <cstdint>
#include <emlabcpp/experimental/function_view.h>

namespace servio::drv
{

struct flash_storage : public storage_iface
{
        flash_storage(
            std::span< em::view< std::byte* >, 2 > pages,
            em::function_view< void() >            start_cb,
            em::function_view< void() >            stop_cb )
          : _pages( pages )
          , _start_cb( start_cb )
          , _stop_cb( stop_cb )
        {
        }

        status store_cfg( cfg::map const& m ) override;
        status load_cfg( cfg::map& m ) override;
        status clear_cfg() override;

private:
        std::span< em::view< std::byte* > > _pages;
        em::function_view< void() >         _start_cb;
        em::function_view< void() >         _stop_cb;
};

}  // namespace servio::drv
