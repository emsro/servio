#include "drv/interfaces.hpp"
#include "fw/util.hpp"
#include "platform.hpp"

#pragma once

namespace servio::drv
{

class dts_temp : public drv::temp_iface
{
public:
        dts_temp( DTS_HandleTypeDef& h )
          : h_( h )
        {
        }

        // TODO: switch to irqs and use sentry

        void tick() override
        {
                if ( ticks_ == 0 ) {
                        HAL_StatusTypeDef s = HAL_DTS_Start( &h_ );
                        if ( s != HAL_OK )
                                fw::stop_exec();

                        if ( HAL_DTS_GetTemperature( &h_, &temp_ ) != HAL_OK )
                                fw::stop_exec();

                        if ( HAL_DTS_Stop( &h_ ) != HAL_OK )
                                fw::stop_exec();

                        ticks_ = ticks_max;
                } else
                        ticks_--;
        }

        int32_t get_temperature() const override
        {
                return temp_;
        }

private:
        int32_t                      temp_;
        DTS_HandleTypeDef&           h_;
        static constexpr std::size_t ticks_max = 512;
        std::size_t                  ticks_    = 0;
};

}  // namespace servio::drv
