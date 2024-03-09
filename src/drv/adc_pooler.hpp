#include "drv/callbacks.hpp"
#include "platform.hpp"
#include "sntr/sentry.hpp"

#pragma once

namespace servio::drv
{

enum adc_error_codes
{
        ADC_POOLER_ISTART_ERR       = 0x1,
        ADC_POOLER_ISTOP_ERR        = 0x2,
        ADC_POOLER_DSTART_ERR       = 0x3,
        ADC_POOLER_DSTOP_ERR        = 0x4,
        ADC_POOLER_CFG_ERR          = 0x5,
        ADC_POOLER_NO_SAMPLES_ERR   = 0x6,
        ADC_POOLER_SAMPLES_OVER_ERR = 0x7
};

inline empty_adc_detailed_cb EMPTY_ADC_DETAILED_CALLBACK;

template < auto ID, std::size_t N >
struct detailed_adc_channel
{
        static constexpr auto id = ID;

        sntr::sentry sentry_;

        alignas( uint32_t ) uint16_t buffer[N];
        std::size_t used;

        uint32_t                   last_value;
        ADC_ChannelConfTypeDef     chconf   = {};
        adc_detailed_cb_interface* callback = &EMPTY_ADC_DETAILED_CALLBACK;

        [[gnu::flatten]] void period_start( ADC_HandleTypeDef& h )
        {
                if ( HAL_ADC_ConfigChannel( &h, &chconf ) != HAL_OK )
                        sentry_.set_inoperable( ADC_POOLER_CFG_ERR, "hal cfg err", ID );
                if ( HAL_ADC_Start_DMA( &h, reinterpret_cast< uint32_t* >( &buffer ), N ) !=
                     HAL_OK ) {
                        sentry_.set_inoperable( ADC_POOLER_DSTART_ERR, "hal dstart err", ID );
                }
        }

        [[gnu::flatten]] void period_stop( ADC_HandleTypeDef& h )
        {
                used = N - __HAL_DMA_GET_COUNTER( h.DMA_Handle );
                if ( HAL_ADC_Stop_DMA( &h ) != HAL_OK )
                        sentry_.set_inoperable( ADC_POOLER_DSTOP_ERR, "hal dstop err", ID );
                if ( used == 0 )
                        sentry_.set_degraded( ADC_POOLER_NO_SAMPLES_ERR, "no samples taken" );
                else if ( used == N )
                        sentry_.set_degraded( ADC_POOLER_SAMPLES_OVER_ERR, "too many samples", N );

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

        sntr::sentry sentry_;

        uint32_t               last_value;
        ADC_ChannelConfTypeDef chconf = {};

        [[gnu::flatten]] void period_start( ADC_HandleTypeDef& h )
        {
                if ( HAL_ADC_ConfigChannel( &h, &chconf ) != HAL_OK )
                        sentry_.set_inoperable( ADC_POOLER_CFG_ERR, "hal cfg err", ID );
                if ( HAL_ADC_Start_IT( &h ) != HAL_OK )
                        sentry_.set_inoperable( ADC_POOLER_ISTART_ERR, "hal istart err", ID );
        }

        void period_stop( ADC_HandleTypeDef& )
        {
        }

        [[gnu::flatten]] void conv_cplt( ADC_HandleTypeDef& h )
        {
                // is this necessary?
                if ( HAL_ADC_Stop_IT( &h ) != HAL_OK )
                        sentry_.set_inoperable( ADC_POOLER_ISTOP_ERR, "hal istop err", ID );
                last_value = HAL_ADC_GetValue( &h );
        }
};

template < auto ID >
struct adc_channel_with_callback : adc_channel< ID >
{
        adc_channel_with_callback( const char* id, sntr::central_sentry_iface& central )
          : adc_channel< ID >( { id, central } )
        {
        }

        value_cb_interface* callback = &EMPTY_ADC_CALLBACK;

        [[gnu::flatten]] void conv_cplt( ADC_HandleTypeDef& h )
        {
                adc_channel< ID >::conv_cplt( h );
                callback->on_value_irq( this->last_value );
        }
};

template < typename Set >
struct adc_pooler
{
        using id_type = typename Set::id_type;

        adc_pooler(
            em::view< const id_type* > seq,
            ADC_HandleTypeDef&         adc,
            DMA_HandleTypeDef&         dma,
            TIM_HandleTypeDef&         tim )
          : adc_( &adc )
          , dma_( &dma )
          , tim_( &tim )
          , sequence_( seq )
        {
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
                if ( h != adc_ )
                        return;
                std::ignore = h;
                // TODO: convert this into flag
        }

        [[gnu::flatten]] void adc_conv_cplt_irq( ADC_HandleTypeDef* h )
        {
                if ( h != adc_ )
                        return;
                const auto active_id = sequence_[sequence_i_];
                with( active_id, [&]( auto& item ) {
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
                        if ( T::id == set_id )
                                f( item );
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
