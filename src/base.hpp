#include "scaled.hpp"

#include <chrono>
#include <cstdint>
#include <emlabcpp/algorithm.h>
#include <numbers>

#pragma once

namespace em = emlabcpp;

using microseconds = std::chrono::duration< uint32_t, std::micro >;
using milliseconds = std::chrono::duration< uint32_t, std::milli >;
using seconds      = std::chrono::duration< uint32_t >;

constexpr microseconds operator""_s( unsigned long long int secs )
{
        return microseconds{ static_cast< uint32_t >( secs * 1'000'000u ) };
}

constexpr microseconds operator""_ms( unsigned long long int msecs )
{
        return microseconds{ static_cast< uint32_t >( msecs * 1'000u ) };
}

constexpr microseconds operator""_us( unsigned long long int usecs )
{
        return microseconds{ static_cast< uint32_t >( usecs ) };
}

using sec_time = std::chrono::duration< float >;

template < typename T >
using limits = em::min_max< T >;

static constexpr float infty = std::numeric_limits< float >::infinity();
static constexpr float pi    = std::numbers::pi_v< float >;

enum class control_mode
{
        DISENGAGED,
        POWER,
        CURRENT,
        VELOCITY,
        POSITION
};

enum class control_loop
{
        CURRENT,
        VELOCITY,
        POSITION
};

struct leds_vals
{
        bool    red;
        bool    blue;
        uint8_t yellow;
        uint8_t green;
};

class current_cb_interface
{
public:
        virtual void on_current( uint32_t current, std::span< uint16_t > profile ) = 0;
        virtual ~current_cb_interface()                                            = default;
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

class position_cb_interface
{
public:
        virtual void on_position( uint32_t pos ) = 0;
        virtual ~position_cb_interface()         = default;
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

class period_cb_interface
{
public:
        virtual void on_period()       = 0;
        virtual ~period_cb_interface() = default;
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
