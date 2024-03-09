
#include "base/base.hpp"
#include "drv/bits/cobs_rx_container.h"

#include <gtest/gtest.h>

namespace std
{
ostream& operator<<( ostream& os, byte b )
{
        return os << static_cast< int >( b );
}
}  // namespace std

namespace servio::drv
{

using namespace base::literals;

TEST( COBS, cont )
{
        std::vector< std::vector< std::byte > > data{
            { 0x1_b, 0x2_b },
            { 0x3_b, 0x4_b },
            { 0x4_b, 0x5_b, 0x6_b },
        };

        bits::cobs_rx_container cont;

        for ( const std::vector< std::byte >& msg : data ) {
                std::array< std::byte, 32 > buffer;
                auto [succes, sub] = em::encode_cobs( msg, buffer );
                EXPECT_TRUE( succes );
                for ( std::byte b : sub )
                        bits::on_rx_cplt_irq( cont, b );
                bits::on_rx_cplt_irq( cont, 0x0_b );
        }

        EXPECT_EQ( cont.sizes.size(), data.size() );

        for ( const std::vector< std::byte >& msg : data ) {
                std::array< std::byte, 16 > buffer;
                auto [success, data] = bits::load_message( cont, buffer );

                EXPECT_TRUE( success );

                auto data2 = em::data_view( msg );
                EXPECT_EQ( data, data2 );
        }
}

}  // namespace servio::drv
