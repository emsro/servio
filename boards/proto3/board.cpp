
#include "brd.hpp"
#include "cfg/default.hpp"
#include "core_drivers.hpp"
#include "fw/drv/adc_pooler.hpp"
#include "fw/drv/clock.hpp"
#include "fw/drv/cobs_uart.hpp"
#include "fw/drv/hbridge.hpp"
#include "fw/drv/leds.hpp"
#include "platform.hpp"
#include "setup.hpp"

#include <emlabcpp/result.h>

namespace em = emlabcpp;

// TODO: this needs it's own header
extern "C" {
extern int _config_start;
extern int _config_end;
}

namespace brd
{


// TODO: well this was copied as is, maybe it can be re-usable?
enum chan_ids
{
        CURRENT_CHANNEL,
        POSITION_CHANNEL,
        VCC_CHANNEL,
        TEMP_CHANNEL,
};

struct adc_set
{
        using id_type = chan_ids;

        fw::drv::detailed_adc_channel< CURRENT_CHANNEL, 128 >  current;
        fw::drv::adc_channel_with_callback< POSITION_CHANNEL > position;
        fw::drv::adc_channel< VCC_CHANNEL >                    vcc;
        fw::drv::adc_channel< TEMP_CHANNEL >                   temp;

        auto tie()
        {
                return std::tie( current, position, vcc, temp );
        }
};

fw::drv::clock                 CLOCK{};
fw::drv::adc_pooler< adc_set > ADC_POOLER{};
fw::drv::cobs_uart             COMMS{};
fw::drv::cobs_uart             DEBUG_COMMS{};
fw::drv::hbridge               HBRIDGE{};
fw::drv::leds                  LEDS;

em::result setup_board()
{
        if ( HAL_Init() != HAL_OK ) {
                return em::ERROR;
        }
        return plt::setup_clk();
}

core_drivers setup_core_drivers()
{
        return core_drivers{
            .start_cb = +[]( core_drivers& ) -> em::result {
                    return em::SUCCESS;
            },
        };
}

cfg::map get_default_config()
{
        return plt::get_default_config();
}


// TODO: this needs better placement
// TODO: study this: https://github.com/littlefs-project/littlefs
em::view< std::byte* > page_at( uint32_t i )
{
        return em::view_n(
            reinterpret_cast< std::byte* >( &_config_start ) + i * FLASH_SECTOR_SIZE,
            FLASH_SECTOR_SIZE );
}
// TODO: one block is small, need more
std::array< em::view< std::byte* >, 1 > PERSISTENT_BLOCKS{ page_at( 0 ) };

em::view< const page* > get_persistent_pages()
{
        return PERSISTENT_BLOCKS;
}

}  // namespace brd
