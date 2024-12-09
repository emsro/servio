#pragma once

#include "./base.hpp"

namespace servio::sntr
{

struct central_sentry_iface
{
        virtual bool is_inoperable() const = 0;

        virtual void report_inoperable(
            char const*      src,
            ecode_set        ecodes,
            char const*      emsg,
            data_type const& data ) = 0;

        virtual void report_degraded(
            char const*      src,
            ecode_set        ecodes,
            char const*      emsg,
            data_type const& data ) = 0;

        virtual ~central_sentry_iface() = default;
};

}  // namespace servio::sntr
