
#include "./paged_i2c_eeprom.hpp"

#include "../fw/util.hpp"

namespace servio::drv
{

namespace
{

em::result write_data(
    I2C_HandleTypeDef*           hi2c,
    uint16_t                     addr,
    uint16_t                     mem_addr,
    std::span< std::byte const > data )
{
        static constexpr std::size_t packet  = 32;
        static constexpr uint32_t    timeout = 200;

        // XXX maybe nonblocking next?
        while ( !data.empty() ) {
                auto n    = (uint16_t) std::min( data.size(), packet );
                auto stat = HAL_I2C_Mem_Write(
                    hi2c, addr, mem_addr, 2, (uint8_t*) data.data(), n, timeout );
                if ( stat != HAL_OK )
                        return em::ERROR;
                data = data.subspan( 0, n );
        }
        return em::SUCCESS;
}

em::result read_data(
    I2C_HandleTypeDef*           hi2c,
    uint16_t                     addr,
    uint16_t                     mem_addr,
    std::span< std::byte const > data )
{
        static constexpr std::size_t packet  = 32;
        static constexpr uint32_t    timeout = 200;

        // XXX maybe nonblocking next?
        while ( !data.empty() ) {
                auto n = (uint16_t) std::min( data.size(), packet );
                auto stat =
                    HAL_I2C_Mem_Read( hi2c, addr, mem_addr, 2, (uint8_t*) data.data(), n, timeout );
                if ( stat != HAL_OK )
                        return em::ERROR;
                data = data.subspan( 0, n );
        }
        return em::SUCCESS;
}

}  // namespace

em::result i2c_eeprom::start()
{
        for ( uint16_t x = 0x00; x < 255; x++ ) {
                uint8_t data[1] = { 0x00 };
                auto    stat    = HAL_I2C_Mem_Read( i2c_, x, 0x00, 2, data, 1, 200 );
                if ( stat == HAL_OK )
                        fw::stop_exec();
        }
        fw::stop_exec();

        uint16_t mem_addr = 0x00;
        for ( ; mem_addr + page_size_ < mem_size_; mem_addr += page_size_ ) {
                paging::state st;

                auto stat = HAL_I2C_Mem_Read( i2c_, addr_, mem_addr, 2, (uint8_t*) &st, 1, 200 );
                if ( stat != HAL_OK )
                        return em::ERROR;

                if ( mem_addr == 0x00 )
                        current_state_ = st;
                else if ( st != current_state_ )
                        break;
        }
        if ( mem_addr + page_size_ >= mem_size_ )
                mem_addr = 0x00;
        page_addr_ = mem_addr;

        return em::SUCCESS;
}

em::result i2c_eeprom::store_page( std::span< std::byte const > data )
{
        if ( data.size() > page_size_ )
                return em::ERROR;
        page_addr_ += page_size_;
        std::byte page_state[1];
        if ( page_addr_ + page_size_ > mem_size_ ) {
                page_addr_    = 0x00;
                page_state[0] = { paging::next( current_state_ ) };
        } else {
                page_state[0] = { current_state_ };
        }
        return write_data( i2c_, addr_, page_addr_, page_state ) &&
               write_data( i2c_, addr_ + 1, page_addr_, data );
}

em::outcome i2c_eeprom::load_page( std::span< std::byte > data )
{
        if ( data.size() > page_size_ )
                return em::ERROR;
        return read_data( i2c_, addr_, page_addr_ + 1, data );
}

}  // namespace servio::drv
