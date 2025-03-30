#pragma once

#include "../lib/paging.hpp"
#include "../plt/platform.hpp"
#include "./interfaces.hpp"

namespace servio::drv
{

// XXX: unit tests for various internal mechanisms?
// XXX: maybe abstract away the paging mechanism?
struct i2c_eeprom : public storage_iface
{
        i2c_eeprom( uint8_t addr, uint16_t size, uint16_t page_size, I2C_HandleTypeDef& i2c )
          : mem_size_( size )
          , page_size_( page_size )
          , addr_( addr )
          , i2c_( &i2c )
        {
        }

        status start();

        status store_page( std::span< std::byte const > data ) override;

        status load_page( std::span< std::byte > data ) override;

private:
        std::size_t        mem_size_;
        paging::state      current_state_;
        uint16_t           page_size_ = 0x00;
        uint16_t           page_addr_ = 0x00;
        uint8_t            addr_;
        I2C_HandleTypeDef* i2c_ = nullptr;
};

}  // namespace servio::drv
