#include "./status.hpp"

namespace servio
{

error_status::operator status() const noexcept
{
        asm( "nop" );
        return status( status_e::ERROR );
}

}  // namespace servio
