#pragma once

#include "base/drv_interfaces.hpp"
#include "sntr/base.hpp"
#include "sntr/record.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <span>

namespace em = emlabcpp;

namespace servio::sntr
{

class central_sentry
{
public:
        central_sentry(
            base::clk_interface&        clk,
            std::span< record >         inop_buffer,
            std::span< record >         degr_buffer,
            em::function_view< void() > stop_callback );

        bool is_inoperable() const;

        void report_inoperable(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data );

        void report_degraded(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data );

private:
        void fire_inoperable();

        bool is_inoperable_ = false;

        base::clk_interface&        clk_;
        std::span< record >         inop_buffer_;
        std::span< record >         degr_buffer_;
        em::function_view< void() > stop_callback_;

        std::size_t inop_i_ = 0;
        std::size_t degr_i_ = 0;
};

}  // namespace servio::sntr
