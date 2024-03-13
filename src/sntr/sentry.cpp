#include "sntr/sentry.hpp"

namespace servio::sntr
{
sentry::sentry( const char* source_id, central_sentry_iface& central )
  : source_id_( source_id )
  , central_( central )
{
        if ( source_id_ == nullptr )
                source_id_ = "unknown";
}

void sentry::set_inoperable( std::size_t eid, const char* emsg, const data_type& data )
{
        if ( eid > max_eid )
                report_inop( forbidden_eid, "inop eid err", static_cast< uint32_t >( eid ) );
        else
                report_inop( eid, emsg, data );
}

void sentry::set_inoperable_set( ecode_set set, const char* emsg, const data_type& data )
{
        central_.report_inoperable( source_id_, set, emsg, data );
}

void sentry::set_degraded( std::size_t eid, const char* emsg, const data_type& data )
{
        if ( eid > max_eid ) {
                report_inop( forbidden_eid, "degr eid err", static_cast< uint32_t >( eid ) );
        } else {
                degr_ecode_.set( eid );
                central_.report_degraded( source_id_, degr_ecode_, emsg, data );
        }
}

void sentry::set_degraded_set( ecode_set set, const char* emsg, const data_type& data )
{
        degr_ecode_ &= set;
        central_.report_degraded( source_id_, degr_ecode_, emsg, data );
}

void sentry::unset_degraded( std::size_t eid )
{
        degr_ecode_.reset( eid );
        central_.report_degraded(
            source_id_, degr_ecode_, "unset degraded", static_cast< uint32_t >( eid ) );
}

void sentry::report_inop( std::size_t eid, const char* emsg, const data_type& data )
{
        inop_ecode_.set( std::min( eid, inop_ecode_.size() - 1 ) );
        central_.report_inoperable( source_id_, inop_ecode_, emsg, data );
}

sentry::~sentry()
{
        set_inoperable( terminated_eid, "sentry terminated" );
}

}  // namespace servio::sntr
