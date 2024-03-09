#pragma once

#include "sntr/base.hpp"

namespace servio::sntr
{

struct central_sentry_iface
{
        virtual bool is_inoperable() const = 0;

        virtual void report_inoperable(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data ) = 0;

        virtual void report_degraded(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data ) = 0;

        virtual ~central_sentry_iface() = default;
};

}  // namespace servio::sntr
