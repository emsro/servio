#include "../platform.hpp"

#include <emlabcpp/view.h>

namespace em = emlabcpp;

extern "C" {
extern int _config_start;
extern int _config_end;
}

namespace servio
{

em::view< std::byte* > page_at( uint32_t i )
{
        return em::view_n(
            reinterpret_cast< std::byte* >( &_config_start ) + i * FLASH_SECTOR_SIZE,
            FLASH_SECTOR_SIZE );
}

}  // namespace servio
