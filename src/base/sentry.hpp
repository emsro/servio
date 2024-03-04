#pragma once

#include "base/base.hpp"
#include "drv/clock.hpp"
#include "emlabcpp/experimental/function_view.h"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <emlabcpp/experimental/string_buffer.h>
#include <span>
#include <string_view>
#include <tuple>
#include <variant>

namespace servio::base
{

static constexpr std::size_t max_eid = 31;

using ecode_set   = std::bitset< max_eid + 1 >;
using sentry_data = std::variant< std::monostate, uint32_t, std::bitset< 32 > >;

enum class record_state
{
        LOCKED,
        SET,
        UNSET,
};

struct record
{
        record_state st = record_state::UNSET;
        microseconds tp;
        const char*  src;
        ecode_set    ecodes;
        const char*  emsg;
        sentry_data  data;
};

static_assert( sizeof( record ) == 28, "Size of record has to be fixed" );

class central_sentry
{
public:
        central_sentry(
            drv::clock&                 clk,
            std::span< record >         inop_buffer,
            std::span< record >         degr_buffer,
            em::function_view< void() > stop_callback )
          : clk_( clk )
          , inop_buffer_( inop_buffer )
          , degr_buffer_( degr_buffer )
          , stop_callback_( stop_callback )
        {
        }

        bool is_inoperable()
        {
                return is_inoperable_;
        }

        void report_inoperable(
            const char*        src,
            ecode_set          ecodes,
            const char*        emsg,
            const sentry_data& data )
        {
                microseconds now = clk_.get_us();
                is_inoperable_   = true;
                record* target   = get_next_free( inop_buffer_, inop_i_ );
                target->st       = record_state::SET;
                target->tp       = now;
                target->src      = src;
                target->ecodes   = ecodes;
                target->emsg     = emsg;
                target->data     = data;
        }

        void report_degraded(
            const char*        src,
            ecode_set          ecodes,
            const char*        emsg,
            const sentry_data& data )
        {
                microseconds now = clk_.get_us();
                is_inoperable_   = true;
                record* target   = get_next_free( degr_buffer_, degr_i_ );
                target->st       = record_state::SET;
                target->tp       = now;
                target->src      = src;
                target->ecodes   = ecodes;
                target->emsg     = emsg;
                target->data     = data;
        }

private:
        static record* get_next_free( std::span< record > buffer, std::size_t& index )
        {
                record* target;
                do {
                        target = &buffer[index];
                        index  = ( index + 1 ) % buffer.size();
                } while ( target->st == record_state::LOCKED );
                return target;
        }

        bool is_inoperable_ = false;

        drv::clock&                 clk_;
        std::span< record >         inop_buffer_;
        std::span< record >         degr_buffer_;
        em::function_view< void() > stop_callback_;

        std::size_t inop_i_ = 0;
        std::size_t degr_i_ = 0;
};

class sentry
{
        // design invariant:
        // once this is set inoperable it can't never recover
public:
        sentry( const char* source_id, central_sentry& central )
          : source_id_( source_id )
          , central_( central )
        {
        }

        void set_inoperable(
            std::size_t        eid,
            const char*        emsg,
            const sentry_data& data = std::monostate{} )
        {
                inop_ecode_.set( std::min( eid, inop_ecode_.size() ) );
                is_inoperable_ = true;

                central_.report_inoperable( source_id_, inop_ecode_, emsg, data );
        }

        void set_degraded(
            std::size_t        eid,
            const char*        emsg,
            const sentry_data& data = std::monostate{} )
        {
                if ( eid > max_eid ) {
                        set_inoperable( inop_ecode_.size(), "eid err" );
                        return;
                }
                degr_ecode_.set( eid );
                central_.report_degraded( source_id_, degr_ecode_, emsg, data );
        }

        void unset_degraded( uint8_t eid )
        {
                degr_ecode_.reset( eid );
                central_.report_degraded(
                    source_id_, degr_ecode_, "unset degraded", static_cast< uint32_t >( eid ) );
        }

private:
        const char*     source_id_;
        central_sentry& central_;

        ecode_set inop_ecode_;
        ecode_set degr_ecode_;
        bool      is_inoperable_ = false;
};

}  // namespace servio::base
