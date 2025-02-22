#pragma once

#include <gtest/gtest.h>

namespace servio::scmdio
{
inline void handle_eptr( std::exception_ptr eptr )
{
        EXPECT_NO_THROW( {
                if ( eptr )
                        std::rethrow_exception( eptr );
        } );
}
}  // namespace servio::scmdio
