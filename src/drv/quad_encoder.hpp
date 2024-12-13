#pragma once

#include "../plt/platform.hpp"
#include "./interfaces.hpp"

namespace servio::drv
{

class quad_encoder final : public pos_iface
{
public:
        quad_encoder( TIM_HandleTypeDef& h )
          : h_( h )
        {
        }

        void start()
        {
                HAL_TIM_Encoder_Start( &h_, TIM_CHANNEL_ALL );
        }

        limits< uint32_t > get_position_range() const override
        {
                return { 0, h_.Init.Period };
        }

        uint32_t get_position() const override
        {
                return __HAL_TIM_GET_COUNTER( &h_ );
        }

        void on_tick_irq()
        {
                if ( irq_counter_ == 0 ) {
                        callback_->on_value_irq( __HAL_TIM_GET_COUNTER( &h_ ) );
                        irq_counter_ = irq_count_max_;
                } else
                        irq_counter_--;
        }

        void set_position_callback( position_cb_iface& cb ) override
        {
                callback_ = &cb;
        }

        position_cb_iface& get_position_callback() const override
        {
                return *callback_;
        }

private:
        static constexpr std::size_t irq_count_max_ = 8;
        std::size_t                  irq_counter_   = 0;
        TIM_HandleTypeDef&           h_;
        value_cb_iface*              callback_ = &EMPTY_CALLBACK;
};

}  // namespace servio::drv
