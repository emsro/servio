
#include "./central_sentry.hpp"

#include "./base.hpp"
#include "./record.hpp"

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
    drv::clk_iface&             clk,
    std::span< record >         inop_buffer,
    std::span< record >         degr_buffer,
    em::function_view< void() > stop_callback )
  : clk_( clk )
  , inop_buffer_( inop_buffer )
  , degr_buffer_( degr_buffer )
  , stop_callback_( stop_callback )
{
        for ( record& r : inop_buffer )
                r = default_record();

        for ( record& r : degr_buffer )
                r = default_record();

        if ( inop_buffer_.empty() )
                fire_inoperable();
        if ( degr_buffer_.empty() )
                fire_inoperable();
}

void central_sentry::fire_inoperable()
{
        if ( is_inoperable_ )
                return;
        is_inoperable_ = true;
        stop_callback_();
}

bool central_sentry::is_inoperable() const
{
        return is_inoperable_;
}

void central_sentry::report_inoperable(
    char const*      src,
    ecode_set        ecodes,
    char const*      emsg,
    data_type const& data )
{
        microseconds now    = clk_.get_us();
        record*      target = get_next_free( inop_buffer_, inop_i_ );
        if ( target ) {
                target->st     = record_state::SET;
                target->tp     = now;
                target->src    = src;
                target->ecodes = (uint32_t) ecodes.to_ulong();
                target->emsg   = emsg;
                target->data   = data;
        }
        fire_inoperable();
}

void central_sentry::report_degraded(
    char const*      src,
    ecode_set        ecodes,
    char const*      emsg,
    data_type const& data )
{
        microseconds now    = clk_.get_us();
        record*      target = get_next_free( degr_buffer_, degr_i_ );
        if ( target ) {
                target->st     = record_state::SET;
                target->tp     = now;
                target->src    = src;
                target->ecodes = (uint32_t) ecodes.to_ulong();
                target->emsg   = emsg;
                target->data   = data;
        }
}

}  // namespace servio::sntr
