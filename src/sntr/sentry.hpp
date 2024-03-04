#pragma once

#include "sntr/base.hpp"

#include <variant>

namespace servio::sntr
{

class central_sentry;

class sentry
{
        // design invariant:
        // once this is set inoperable it can't never recover
public:
        sentry( const char* source_id, central_sentry& central );

        void set_inoperable(
            std::size_t      eid,
            const char*      emsg,
            const data_type& data = std::monostate{} );

        void
        set_degraded( std::size_t eid, const char* emsg, const data_type& data = std::monostate{} );

        void unset_degraded( uint8_t eid );

private:
        const char*     source_id_;
        central_sentry& central_;

        ecode_set inop_ecode_;
        ecode_set degr_ecode_;
        bool      is_inoperable_ = false;
};

}  // namespace servio::sntr
