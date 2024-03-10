#pragma once

#include "sntr/central_sentry_iface.hpp"

#include <emlabcpp/static_vector.h>
#include <tuple>

namespace em = emlabcpp;

namespace servio::sntr
{

struct test_central_sentry : central_sentry_iface
{
        enum src
        {
                INOP,
                DEGR
        };

        using record = std::tuple< src, const char*, ecode_set, const char*, data_type >;
        em::static_vector< record, 8 > buffer;
        bool                           is_inop = false;

        bool is_inoperable() const override
        {
                return is_inop;
        }

        void report_inoperable(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data ) override
        {
                is_inop = true;
                if ( !buffer.full() )
                        buffer.emplace_back( INOP, src, ecodes, emsg, data );
        }

        void report_degraded(
            const char*      src,
            ecode_set        ecodes,
            const char*      emsg,
            const data_type& data ) override
        {
                if ( !buffer.full() )
                        buffer.emplace_back( DEGR, src, ecodes, emsg, data );
        }
};

}  // namespace servio::sntr
