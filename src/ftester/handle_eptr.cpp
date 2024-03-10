#include "ftester/handle_eptr.hpp"

#include <emlabcpp/experimental/logging.h>

namespace servio::ftester
{

void handle_eptr( std::exception_ptr eptr )
{
        try {
                if ( eptr )
                        std::rethrow_exception( eptr );
        }
        catch ( const std::exception& e ) {
                EMLABCPP_ERROR_LOG( "Caught exception: '", e.what(), "'\n" );
        }
        catch ( ... ) {
                EMLABCPP_ERROR_LOG( "Caught an unknown exception'\n" );
        }
}
}  // namespace servio::ftester
