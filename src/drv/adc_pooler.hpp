#include "drv/callbacks.hpp"
#include "platform.hpp"

#pragma once

namespace servio::drv
{
inline empty_adc_detailed_cb EMPTY_ADC_DETAILED_CALLBACK;

template < auto ID, std::size_t N >
struct detailed_adc_channel
{
        static constexpr auto id = ID;

        alignas( uint32_t ) uint16_t buffer[N];
        std::size_t used;

        uint32_t                         last_value;
        ADC_ChannelConfTypeDef           chconf   = {};
        base::adc_detailed_cb_interface* callback = &EMPTY_ADC_DETAILED_CALLBACK;

        bool config_channel_error = false;
        bool start_error          = false;
        bool stop_error           = false;

        bool no_samples_error       = false;
        bool samples_overflow_error = false;

        base::status get_status() const
        {
                if ( config_channel_error || start_error || stop_error ) {
                        return base::status::INOPERABLE;
                }
                if ( no_samples_error || samples_overflow_error ) {
                        return base::status::DEGRADED;
                }
                return base::status::NOMINAL;
        }

        void clear_status()
        {
                no_samples_error       = false;
                samples_overflow_error = false;
        }

        void period_start( ADC_HandleTypeDef& h )
        {
                if ( HAL_ADC_ConfigChannel( &h, &chconf ) != HAL_OK ) {
                        config_channel_error = true;
                }
                if ( HAL_ADC_Start_DMA( &h, reinterpret_cast< uint32_t* >( &buffer ), N ) !=
                     HAL_OK ) {
                        start_error = true;
                }
        }

        [[gnu::flatten]] void period_stop( ADC_HandleTypeDef& h )
        {
                used = N - __HAL_DMA_GET_COUNTER( h.DMA_Handle );
                if ( HAL_ADC_Stop_DMA( &h ) != HAL_OK ) {
                        stop_error = true;
                }
                if ( used == 0 ) {
                        no_samples_error = true;

                } else if ( used == N ) {
                        samples_overflow_error = false;
                }

                const std::span readings( std::data( buffer ), used );

                last_value = em::avg( readings );
                callback->on_value_irq( last_value, readings );
        }

        void conv_cplt( ADC_HandleTypeDef& )
        {
        }
};

inline empty_value_cb EMPTY_ADC_CALLBACK;

template < auto ID >
struct adc_channel
{
        static constexpr auto id = ID;

        uint32_t               last_value;
        ADC_ChannelConfTypeDef chconf = {};

        bool config_channel_error = false;
        bool start_error          = false;
        bool stop_error           = false;

        base::status get_status() const
        {
                if ( config_channel_error || start_error || stop_error ) {
                        return base::status::INOPERABLE;
                }
                return base::status::NOMINAL;
        }

        void period_start( ADC_HandleTypeDef& h )
        {
                if ( HAL_ADC_ConfigChannel( &h, &chconf ) != HAL_OK ) {
                        config_channel_error = true;
                }
                if ( HAL_ADC_Start_IT( &h ) != HAL_OK ) {
                        start_error = true;
                }
        }

        void period_stop( ADC_HandleTypeDef& )
        {
        }

        void conv_cplt( ADC_HandleTypeDef& h )
        {
                // is this necessary?
                if ( HAL_ADC_Stop_IT( &h ) != HAL_OK ) {
                        stop_error = true;
                }
                last_value = HAL_ADC_GetValue( &h );
        }
};

template < auto ID >
struct adc_channel_with_callback : adc_channel< ID >
{
        base::value_cb_interface* callback = &EMPTY_ADC_CALLBACK;

        void conv_cplt( ADC_HandleTypeDef& h )
        {
                adc_channel< ID >::conv_cplt( h );
                callback->on_value_irq( this->last_value );
        }
};

template < typename Set >
struct adc_pooler
{
        using id_type = typename Set::id_type;

        adc_pooler() = default;

        adc_pooler* setup(
            em::view< const id_type* > seq,
            ADC_HandleTypeDef&         adc,
            DMA_HandleTypeDef&         dma,
            TIM_HandleTypeDef&         tim )
        {
                adc_      = &adc;
                dma_      = &dma;
                tim_      = &tim;
                sequence_ = seq;
                return this;
        }

        em::result start()
        {
                __HAL_TIM_ENABLE( tim_ );
                return em::SUCCESS;
        }

        Set* operator->()
        {
                return &set_;
        }
        Set& operator*()
        {
                return set_;
        }

        void adc_error_irq( ADC_HandleTypeDef* h )
        {
                if ( h != adc_ ) {
                        return;
                }
                std::ignore = h;
                // TODO: convert this into flag
        }

        [[gnu::flatten]] void adc_conv_cplt_irq( ADC_HandleTypeDef* h )
        {
                if ( h != adc_ ) {
                        return;
                }
                const auto active_id = sequence_[sequence_i_];
                with( active_id, [&]( auto& item ) {
                        // TODO: error handling?
                        item.conv_cplt( *adc_ );
                } );
        }

        [[gnu::flatten]] void on_period_irq()
        {
                auto active_id = sequence_[sequence_i_];

                with( active_id, [&]( auto& item ) {
                        item.period_stop( *adc_ );
                } );

                sequence_i_ = ( sequence_i_ + 1 ) % sequence_.size();
                active_id   = sequence_[sequence_i_];

                with( active_id, [&]( auto& item ) {
                        item.period_start( *adc_ );
                } );
        }

private:
        [[gnu::flatten]] void with( id_type set_id, auto&& f )
        {
                em::for_each( set_.tie(), [&]< typename T >( T& item ) {
                        if ( T::id == set_id ) {
                                f( item );
                        }
                } );
        }

        ADC_HandleTypeDef* adc_ = {};
        DMA_HandleTypeDef* dma_ = {};
        TIM_HandleTypeDef* tim_ = {};

        std::size_t sequence_i_ = 0;

        em::view< const id_type* > sequence_;
        Set                        set_;
};

}  // namespace servio::drv
