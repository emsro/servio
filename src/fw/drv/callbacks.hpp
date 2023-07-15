#include <cstdint>
#include <span>

#pragma once

namespace fw::drv
{

class current_cb_interface
{
public:
        virtual void on_current( uint32_t current, std::span< uint16_t > profile ) = 0;
        virtual ~current_cb_interface()                                            = default;
};

class position_cb_interface
{
public:
        virtual void on_position( uint32_t pos ) = 0;
        virtual ~position_cb_interface()         = default;
};

class period_cb_interface
{
public:
        virtual void on_period()       = 0;
        virtual ~period_cb_interface() = default;
};

template < typename Callable >
struct current_cb : public current_cb_interface
{
        current_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        void on_current( uint32_t current, std::span< uint16_t > profile )
        {
                cb( current, profile );
        }

        Callable cb;
};

struct empty_current_cb : public current_cb_interface
{
        void on_current( uint32_t, std::span< uint16_t > )
        {
        }
};

template < typename Callable >
struct position_cb : public position_cb_interface
{
        position_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        virtual void on_position( uint32_t pos )
        {
                cb( pos );
        }

        Callable cb;
};

struct empty_position_cb : public position_cb_interface
{
        virtual void on_position( uint32_t )
        {
        }
};

template < typename Callable >
struct period_cb : public period_cb_interface
{
        period_cb( Callable cb )
          : cb( std::move( cb ) )
        {
        }

        virtual void on_period()
        {
                cb();
        }

        Callable cb;
};

struct empty_period_cb : public period_cb_interface
{
        virtual void on_period()
        {
        }
};

}  // namespace fw::drv
