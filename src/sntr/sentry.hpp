#pragma once

#include "sntr/base.hpp"
#include "sntr/central_sentry_iface.hpp"

#include <variant>

namespace servio::sntr
{

class sentry
{
public:
        sentry( const char* source_id, central_sentry_iface& central );

        sentry( const sentry& )            = delete;
        sentry( sentry&& )                 = delete;
        sentry& operator=( const sentry& ) = delete;
        sentry& operator=( sentry&& )      = delete;

        void set_inoperable(
            std::size_t      eid,
            const char*      emsg,
            const data_type& data = std::monostate{} );

        void
        set_degraded( std::size_t eid, const char* emsg, const data_type& data = std::monostate{} );

        void unset_degraded( std::size_t eid );

        ~sentry();

private:
        void report_inop( std::size_t eid, const char* emsg, const data_type& data );

        const char*           source_id_;
        central_sentry_iface& central_;

        ecode_set inop_ecode_;
        ecode_set degr_ecode_;
};

}  // namespace servio::sntr
