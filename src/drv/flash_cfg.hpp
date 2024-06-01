#pragma once

#include "cfg/storage.hpp"
#include "drv/interfaces.hpp"
#include "platform.hpp"

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

        em::result store_page( std::span< const std::byte > data ) override;

        em::outcome load_page( std::span< std::byte > data ) override;

private:
        std::span< em::view< std::byte* > > _pages;
};

}  // namespace servio::drv
