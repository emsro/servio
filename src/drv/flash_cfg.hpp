#pragma once

#include "../cfg/storage.hpp"
#include "../plt/platform.hpp"
#include "./interfaces.hpp"

#include <cstdint>
#include <emlabcpp/defer.h>

namespace servio::drv
{

class flash_storage : public storage_iface
{
public:
        flash_storage( std::span< em::view< std::byte* >, 2 > pages )
          : _pages( pages )
        {
        }

        em::result store_page( std::span< std::byte const > data ) override;

        em::outcome load_page( std::span< std::byte > data ) override;

private:
        std::span< em::view< std::byte* > > _pages;
};

}  // namespace servio::drv
