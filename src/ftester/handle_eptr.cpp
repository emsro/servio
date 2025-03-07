#include "./handle_eptr.hpp"

#include <spdlog/spdlog.h>

namespace servio::ftester
{

// XXX: this might not be the greatest idea ever
void handle_eptr( std::exception_ptr eptr )
{
        try {
                if ( eptr )
                        std::rethrow_exception( eptr );
        }
        catch ( std::exception const& e ) {
                spdlog::dump_backtrace();
                spdlog::error( "Caught an exception: {}", e.what() );
        }
        catch ( ... ) {
                spdlog::dump_backtrace();
                spdlog::error( "Caught an unknown exception" );
        }
}
}  // namespace servio::ftester
