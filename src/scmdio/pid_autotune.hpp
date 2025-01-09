#pragma once

#include "./port.hpp"

#include <emlabcpp/algorithm.h>
#include <emlabcpp/enumerate.h>

namespace servio::scmdio
{

namespace em = emlabcpp;

struct line
{
        double off;
        double scale;
};

// based on https://en.wikipedia.org/wiki/Linear_least_squares
// We want to fit a line to provided dataset `ys`, model is a following equation:
//
//  a + i*b + e_i = y_i
//
// where `a` is offset of the line, `b` is scale of the line, `i` is index of measurement, `e_i` is
// unknown error of measurement `i` and `y_i` is input values.
//
// The solution is to find `a,b` such that `S = sum e_i^2` is minimal. To do that let's focus on
// `e_i`:
//
//  e_i = y_i - i*b - a
//
// Given that in the end we work with sum of e_i^2:
//
//  e_i^2 = (y_i - i*b - a)^2 = a^2 + 2*a*i*b - 2*a*y_i - (b*i)^2 - 2*i*b*y_i + (y_i)^2
//
// Reformulating, goal is to minimize:
//
//  S = sum a^2 + 2*a*i*b - 2*a*y_i - (b*i)^2 - 2*i*b*y_i + (y_i)^2
//
// Given that we minimize relative to vars `a` and `b`, we want the derivation of the sum in respect
// to these equal to 0:
//
//  0 = derive S by a = sum 2a + 2ib - 2y_i
//  0 = derive S by b = sum 2ai + 2b(i^2) - 2iy_i
//
// The sum can be summed up:
//
//  si = sum i
//  sii = sum i^2
//  sy = sum y_i
//  siy = sum i*y_i
//
//  0 = 2*a*n + 2*b*si - 2*sy
//  0 = 2*a*si + 2*b*sii - 2*siy
//
//  0 = a*n + b*si - sy
//  0 = a*si + b*sii - siy
//
// express `b`:
//
// a*n = sy - b*si
// a*si = siy - b*sii
//
// a = (sy-b*si)/n
// a = (siy - b*sii)/si
//
// (sy-b*si)/n = (siy - b*sii)/si
//
// si*(sy-b*si) = n*(siy - b*sii)
//
// si*sy - si*b*si = n*siy - n*b*sii
//
// n*b*sii - si*b*si = n*siy - si*sy
// b * (n*sii - si*si) = n*siy - si*sy
//
// b = (n*siy - si*sy)/(n*sii - si*si)

inline line linear_least_squares( std::span< double > ys )
{
        auto   n   = static_cast< double >( ys.size() );
        double si  = n * ( n + 1.0 ) / 2.0;
        double sii = n * ( n + 1 ) * ( 2 * n + 1 ) / 6.0;
        double sy  = em::sum( ys );
        double siy = 0.0;
        for ( auto [i, y] : em::enumerate( ys ) )
                siy += static_cast< double >( i + 1 ) * y;

        double b = ( n * siy - si * sy ) / ( n * sii - si * si );
        double a = ( sy - b * si ) / n;
        return {
            .off   = a,
            .scale = b,
        };
}

awaitable< std::vector< double > > pid_collect( float u, auto&& set_f, auto&& get_y_f )
{
        co_await set_f( u );

        std::vector< double > buff;
        buff.reserve( 1024 );
        for ( int i = 0; i < 1000; i++ )
                buff.push_back( (double) co_await get_y_f() );
        co_return buff;
}

inline void pid_calculate( std::span< double > buff )
{

        em::min_max mm    = em::min_max_elem( buff );
        double      range = mm.max() - mm.min();
        double      from  = mm.min() + range * 0.2;
        double      to    = mm.min() + range * 0.8;

        std::cout << "buff size: " << buff.size() << std::endl;
        std::cout << "range: " << range << "\tfrom: " << from << "\tto:" << to << std::endl;

        auto beg_iter = em::find_if( buff, [&]( double v ) {
                return v > from;
        } );
        auto end_iter = em::find_if( buff, [&]( double v ) {
                return v > to;
        } );

        std::span< double > lls_span{
            &*beg_iter, static_cast< std::size_t >( std::distance( beg_iter, end_iter ) ) };
        std::cout << "lls_span size: " << lls_span.size() << std::endl;
        line guide = linear_least_squares( lls_span );
        std::cout << "guide: " << guide.off << " + " << guide.scale << " * x" << std::endl;
        double K = range;
        double L = -guide.off;
        // y = a + b*x
        // x = (y-a)/b
        double T = ( K - guide.off ) / guide.scale - L;

        std::cout << "K: " << K << "\tL: " << L << "\tT: " << T << std::endl;

        // Cohen coon, inspiration:
        // https://www.incatools.com/pid-tuning/pid-tuning-methods/
        double tau = L / ( L + T );
        double a   = K * L / T;

        double K_p = ( 1.35 / a ) * ( 1 + 0.18 * tau / ( 1 - tau ) );
        double T_i = L * ( 2.5 - 2.0 * tau ) / ( 1 - 0.39 * tau );
        double T_d = L * ( 0.37 - 0.37 * tau ) / ( 1 - 0.81 * tau );
        double K_i = K_p / T_i;
        double K_d = K_p * T_d;
        std::cout << "P: " << K_p << "\tI: " << K_i << "\tD: " << K_d << std::endl;
}

}  // namespace servio::scmdio
