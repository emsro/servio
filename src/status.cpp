#include "./status.hpp"

namespace servio
{

error_status::operator status() const noexcept
{
        asm( "nop" );
        return status( status_e::ERROR );
}

std::string_view to_str( status s )
{
        switch ( s.value() ) {
        case status_e::SUCCESS:
                return "success";
        case status_e::FAILURE:
                return "failure";
        case status_e::ERROR:
                return "error";
        default:
                return "unknown";
        }
}

}  // namespace servio
