#pragma once

#include "ftester/base.hpp"

#include <emlabcpp/convert_view.h>
#include <list>

namespace servio::ftester
{

class recorder
{
public:
        struct record
        {
                std::vector< std::byte > data;
        };

        recorder( em::protocol::channel_type chan )
          : chan_( chan )
        {
        }

        em::protocol::channel_type get_channel() const
        {
                return chan_;
        }

        em::outcome on_msg( const std::span< const std::byte >& msg )
        {
                buffer_.push_back( record{
                    .data = { msg.begin(), msg.end() },
                } );
                return em::SUCCESS;
        }

        void clear()
        {
                buffer_.clear();
        }

        const std::list< record >& get_buffer() const
        {
                return buffer_;
        }

private:
        std::list< record >        buffer_;
        em::protocol::channel_type chan_;
};

static_assert( em::protocol::slot< recorder > );

}  // namespace servio::ftester
