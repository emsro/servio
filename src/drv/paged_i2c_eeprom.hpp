#pragma once

#include "../plt/platform.hpp"
#include "./interfaces.hpp"

namespace servio::drv
{

// XXX: unit tests for various internal mechanisms?
struct i2c_eeprom : public storage_iface
{
        i2c_eeprom(
            clk_iface&         clk,
            uint8_t            dev_addr,
            uint16_t           mem_size,
            uint16_t           page_size,
            I2C_HandleTypeDef& i2c )
          : dev_addr( dev_addr << 1 )
          , clk_( clk )
          , mem_size_( mem_size )
          , page_size_( page_size )
          , i2c_( &i2c )
        {
        }

        uint8_t dev_addr;

        status store_cfg( cfg::map const& m ) override;
        status load_cfg( cfg::map& m ) override;
        status clear_cfg() override;

private:
        clk_iface&         clk_;
        uint16_t           mem_size_;
        uint16_t           page_size_ = 0x00;
        I2C_HandleTypeDef* i2c_       = nullptr;
};

}  // namespace servio::drv
