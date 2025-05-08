#pragma once

#include <vari/vref.h>

namespace servio
{

template < typename... Ts >
struct _type_tag;

template < typename TL >
struct _type_tag_core
{

        constexpr _type_tag_core() noexcept = default;

        template < typename UL >
        constexpr _type_tag_core( _type_tag_core< UL > const& other ) noexcept
          : index( vari::_vptr_cnv_map< TL, UL >::conv( other.get_index() ) )
        {
        }

        template < typename... Fs >
        constexpr decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                return vari::_dispatch_index< 0, TL::size >(
                    index, [&]< auto j >() -> decltype( auto ) {
                            using U = vari::type_at_t< j, TL >;
                            _type_tag< U > val;
                            return vari::_dispatch_fun( val, (Fs&&) fs... );
                    } );
        }

        constexpr vari::index_type get_index() const noexcept
        {
                return index;
        }

        vari::index_type index = 0;
};

template < typename T >
struct _type_tag_core< vari::typelist< T > >
{

        constexpr _type_tag_core() noexcept                                               = default;
        constexpr _type_tag_core( _type_tag_core< vari::typelist< T > > const& ) noexcept = default;

        template < typename... Fs >
        constexpr decltype( auto ) visit_impl( Fs&&... fs ) const
        {
                _type_tag< T > val;
                return vari::_dispatch_fun( val, (Fs&&) fs... );
        }

        constexpr vari::index_type get_index() const noexcept
        {
                return 0;
        }
};

template < typename... Ts >
struct _type_tag
{
        using types = vari::typelist< Ts... >;

        constexpr _type_tag() noexcept = default;

        constexpr _type_tag( _type_tag const& ) = default;

        template < typename... Us >
        constexpr _type_tag( _type_tag< Us... > const& other ) noexcept
          : core( other.core )
        {
        }

        /// Returns the index representing the type currently being referenced.
        /// The index of the first type is 0, with subsequent types sequentially numbered.
        [[nodiscard]] constexpr vari::index_type index() const noexcept
        {
                return core.get_index();
        }

        /// Calls the appropriate function from the list `fs...`, based on the type of the current
        /// target.
        template < typename... Fs >
        constexpr decltype( auto ) visit( Fs&&... fs ) const
        {
                // XXX: fix
                //                typename vari::_check_unique_invocability< types >::template
                //                with_pure_ref< Fs... >
                //                  _{};
                return core.visit_impl( (Fs&&) fs... );
        }

        _type_tag_core< vari::typelist< Ts... > > core;
};

template < typename... Ts >
using type_tag = vari::_define_variadic< _type_tag, vari::typelist< Ts... > >;

template < auto Tag >
using type_tag_type = vari::type_at_t< Tag.index(), typename decltype( Tag )::types >;

}  // namespace servio
