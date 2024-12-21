#pragma once

#include "./base.hpp"
#include "./central_sentry_iface.hpp"

#include <variant>

namespace servio::sntr
{

class sentry
{
public:
        sentry( char const* source_id, central_sentry_iface& central );

        sentry( sentry const& )            = delete;
        sentry( sentry&& )                 = delete;
        sentry& operator=( sentry const& ) = delete;
        sentry& operator=( sentry&& )      = delete;

        void set_inoperable( std::size_t eid, char const* emsg, data_type const& data = {} );

        void set_inoperable_set( ecode_set set, char const* emsg, data_type const& data = {} );

        void set_degraded( std::size_t eid, char const* emsg, data_type const& data = {} );

        void set_degraded_set( ecode_set set, char const* emsg, data_type const& data = {} );

        void unset_degraded( std::size_t eid );

        ~sentry();

private:
        void report_inop( std::size_t eid, char const* emsg, data_type const& data );

        char const*           source_id_;
        central_sentry_iface& central_;

        ecode_set inop_ecode_;
        ecode_set degr_ecode_;
};

}  // namespace servio::sntr
