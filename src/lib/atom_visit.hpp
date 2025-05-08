#pragma once

#include <avakar/atom.h>
#include <vari/dispatch.h>

namespace servio
{
template < std::size_t N >
struct atom_sv_key
{
        constexpr atom_sv_key( std::string_view str ) noexcept
        {
                std::copy( str.begin(), str.end(), _s );
                _s[N - 1] = '\0';
        }

        constexpr std::string_view to_string() const noexcept
        {
                return { _s, N - 1 };
        }

        char _s[N];
};

template < auto sv_key >
struct key_atom_literal
{
        static constexpr std::string_view to_string() noexcept
        {
                return sv_key.to_string();
        }
};

template < auto& k >
using sv_key = key_atom_literal< atom_sv_key< k.size() + 1 >( k ) >;

template < typename K, typename... Fn >
auto atom_visit( K const& key, Fn&&... fn )
{
        return vari::dispatch< K::keys.size() >(
            key.value(),
            [&]< std::size_t i >() {
                    constexpr auto KK = sv_key< K::keys[i] >{};
                    return avakar::atom< KK >{ KK };
            },
            (Fn&&) fn... );
}

}  // namespace servio
