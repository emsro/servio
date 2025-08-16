#pragma once

#include <emlabcpp/result.h>
#include <emlabcpp/status.h>

namespace servio
{
namespace em = emlabcpp;

enum class status_e
{
        SUCCESS = 0,  // all went well
        FAILURE = 1,  // expected error occured
        ERROR   = 2,  // unexpected error occured
};

struct [[nodiscard]] status : em::status< status, status_e >
{
        using base = em::status< status, status_e >;
        using enum status_e;
        using base::status;

        constexpr status( em::result const r ) noexcept
          : base( r == em::result::SUCCESS ? status_e::SUCCESS : status_e::ERROR )
        {
        }

        operator em::result() const noexcept
        {
                return ( value() == status_e::SUCCESS ) ? em::result::SUCCESS : em::result::ERROR;
        }
};

struct error_status
{
        operator status() const noexcept;

        constexpr bool operator==( status const& s ) const noexcept
        {
                return s.value() == status_e::ERROR;
        }
};

static constexpr error_status ERROR;

static constexpr status FAILURE = status::FAILURE;
static constexpr status SUCCESS = status::SUCCESS;

}  // namespace servio
