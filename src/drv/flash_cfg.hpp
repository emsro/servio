#pragma once

#include "../cfg/storage.hpp"
#include "../plt/platform.hpp"
#include "./interfaces.hpp"

#include <cstdint>

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

        status store_page( std::span< std::byte const > data ) override;

        status load_page( std::span< std::byte > data ) override;

private:
        std::span< em::view< std::byte* > > _pages;
        em::function_view< void() >         _start_cb;
        em::function_view< void() >         _stop_cb;
};

}  // namespace servio::drv
