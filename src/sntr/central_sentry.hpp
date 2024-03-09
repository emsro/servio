#pragma once

#include "drv/interfaces.hpp"
#include "sntr/central_sentry_iface.hpp"
#include "sntr/record.hpp"

#include <emlabcpp/experimental/function_view.h>
#include <span>

namespace em = emlabcpp;

namespace servio::sntr
{

class central_sentry : public central_sentry_iface
{
public:
        central_sentry(
            drv::clk_interface&         clk,
            std::span< record >         inop_buffer,
            std::span< record >         degr_buffer,
            em::function_view< void() > stop_callback );

        central_sentry( const central_sentry& )            = delete;
        central_sentry( central_sentry&& )                 = delete;
        central_sentry& operator=( const central_sentry& ) = delete;
        central_sentry& operator=( central_sentry&& )      = delete;

        bool is_inoperable() const override;

        void report_inoperable(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data ) override;

        void report_degraded(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data ) override;

private:
        void fire_inoperable();

        bool is_inoperable_ = false;

        drv::clk_interface&         clk_;
        std::span< record >         inop_buffer_;
        std::span< record >         degr_buffer_;
        em::function_view< void() > stop_callback_;

        std::size_t inop_i_ = 0;
        std::size_t degr_i_ = 0;
};

}  // namespace servio::sntr
