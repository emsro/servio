#include "./interfaces.hpp"

#include <emlabcpp/defer.h>

#pragma once

namespace em = emlabcpp;

namespace servio::drv
{

inline auto retain_callback( period_iface& period )
{
        auto initial_cb_ptr = &period.get_period_callback();
        return em::defer{ [initial_cb_ptr, period_ptr = &period] {
                period_ptr->set_period_callback( *initial_cb_ptr );
        } };
}

inline auto retain_callback( curr_iface& curr_drv )
{
        auto initial_cb_ptr = &curr_drv.get_current_callback();
        return em::defer{ [initial_cb_ptr, curr_ptr = &curr_drv]() mutable {
                curr_ptr->set_current_callback( *initial_cb_ptr );
        } };
}

inline auto retain_callback( pos_iface& pos_drv )
{
        auto initial_cb_ptr = &pos_drv.get_position_callback();
        return em::defer{ [initial_cb_ptr, pos_ptr = &pos_drv]() mutable {
                pos_ptr->set_position_callback( *initial_cb_ptr );
        } };
}

}  // namespace servio::drv
