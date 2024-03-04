#include "sntr/sentry.hpp"

#include "sntr/central_sentry.hpp"

namespace servio::sntr
{
sentry::sentry( const char* source_id, central_sentry& central )
  : source_id_( source_id )
  , central_( central )
{
}

void sentry::set_inoperable( std::size_t eid, const char* emsg, const data_type& data )
{
        inop_ecode_.set( std::min( eid, inop_ecode_.size() ) );
        is_inoperable_ = true;

        central_.report_inoperable( source_id_, inop_ecode_, emsg, data );
}

void sentry::set_degraded( std::size_t eid, const char* emsg, const data_type& data )
{
        if ( eid > max_eid ) {
                set_inoperable( inop_ecode_.size(), "eid err" );
                return;
        }
        degr_ecode_.set( eid );
        central_.report_degraded( source_id_, degr_ecode_, emsg, data );
}

void sentry::unset_degraded( uint8_t eid )
{
        degr_ecode_.reset( eid );
        central_.report_degraded(
            source_id_, degr_ecode_, "unset degraded", static_cast< uint32_t >( eid ) );
}

}  // namespace servio::sntr
