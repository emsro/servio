

#include "../parser.hpp"

#include <gtest/gtest.h>

namespace servio::parser
{

TEST( iface, str )
{

        auto test_f = [&]( std::string_view s, std::string_view exp ) {
                char const* p = s.data();
                char const* e = s.data() + s.size();

                auto tmp = parse_str( p, e );
                EXPECT_EQ( *tmp, exp ) << "\n inpt: " << s << "\n";
        };

        test_f( "\"wololo boo\"", "wololo boo" );
}

}  // namespace servio::parser
