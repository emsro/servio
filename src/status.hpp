#pragma once

#include <cstdint>
#include <emlabcpp/error_code.h>

namespace em = emlabcpp;

namespace servio
{

enum class [[nodiscard]] status : uint8_t
{
        success   = 0,  // all went well
        failure   = 1,  // expected error occured
        error     = 2,  // unexpected error occured
        enter_dfu = 3,  // request to enter DFU bootloader after reply is sent
};

struct status_error_category : em::error_category< status >
{
        [[nodiscard]] char const* message( em::error_value_type code ) const noexcept override
        {
                switch ( static_cast< status >( code ) ) {
                case status::success:
                        return "success";
                case status::failure:
                        return "failure";
                case status::error:
                        return "error";
                case status::enter_dfu:
                        return "enter_dfu";
                default:
                        return "unknown error code";
                }
        }
};

}  // namespace servio

namespace emlabcpp
{
template <>
inline servio::status_error_category const error_category_v< servio::status > = {};
}
