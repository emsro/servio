
#include "../lib/str_lib.h"

#include <string_view>
#include <vari/vopt.h>

namespace servio::iface
{

inline vari::vopt< std::string_view > parse_str( char const*& p, char const* e ) noexcept
{

        char const* pp = p;
        if ( *p != '"' )
                return {};
        while ( pp != e ) {
                pp++;
                if ( str_lib::bits::is_id_letter( *pp ) || *pp == ' ' )
                        continue;
                if ( *pp != '"' )
                        return {};
                std::string_view res{ p + 1, pp };
                p = pp;
                return res;
        }
        return {};
}

}  // namespace servio::iface
