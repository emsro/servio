
#include "sntr/central_sentry.hpp"

#include "base/drv_interfaces.hpp"
#include "sntr/base.hpp"
#include "sntr/record.hpp"

namespace servio::sntr
{

namespace
{
        record* get_next_free( std::span< record > buffer, std::size_t& index )
        {
                record* target;
                do {
                        target = &buffer[index];
                        index  = ( index + 1 ) % buffer.size();
                } while ( target->st == record_state::LOCKED );
                return target;
        }
}  // namespace

central_sentry::central_sentry(
    base::clk_interface&        clk,
    std::span< record >         inop_buffer,
    std::span< record >         degr_buffer,
    em::function_view< void() > stop_callback )
  : clk_( clk )
  , inop_buffer_( inop_buffer )
  , degr_buffer_( degr_buffer )
  , stop_callback_( stop_callback )
{
}

bool central_sentry::is_inoperable() const
{
        return is_inoperable_;
}

void central_sentry::report_inoperable(
    const char*      src,
    ecode_set        ecodes,
    const char*      emsg,
    const data_type& data )
{
        base::microseconds now = clk_.get_us();
        is_inoperable_         = true;
        record* target         = get_next_free( inop_buffer_, inop_i_ );
        target->st             = record_state::SET;
        target->tp             = now;
        target->src            = src;
        target->ecodes         = ecodes;
        target->emsg           = emsg;
        target->data           = data;
}

void central_sentry::report_degraded(
    const char*      src,
    ecode_set        ecodes,
    const char*      emsg,
    const data_type& data )
{
        base::microseconds now = clk_.get_us();
        is_inoperable_         = true;
        record* target         = get_next_free( degr_buffer_, degr_i_ );
        target->st             = record_state::SET;
        target->tp             = now;
        target->src            = src;
        target->ecodes         = ecodes;
        target->emsg           = emsg;
        target->data           = data;
}

}  // namespace servio::sntr
