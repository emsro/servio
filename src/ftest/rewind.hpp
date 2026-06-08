#pragma once

#include "../core/core.hpp"
#include "../drv/interfaces.hpp"
#include "../gov/current/current.hpp"
#include "./utest.hpp"

#include <optional>

namespace servio::ftest
{

namespace em = emlabcpp;

template < typename CB >
struct _rewind_sender
{
        using sender_concept = ecor::sender_t;

        using completion_signatures =
            ecor::completion_signatures< ecor::set_value_t(), ecor::set_error_t( asrt::status ) >;

        utest*               pctx;
        core::core*          pcor;
        drv::clk_iface*      piclk;
        drv::pos_iface*      pipos;
        microseconds         timeout;
        em::min_max< float > area;
        float                current;
        CB                   cb;

        template < ecor::receiver R >
        struct _op : ecor::schedulable
        {
                using operation_state_concept = ecor::operation_state_t;

                struct sub_receiver
                {
                        using receiver_concept = ecor::receiver_t;
                        _op* owner;

                        void set_value()
                        {
                                owner->core->reschedule( *owner );
                        }

                        void set_error( auto e )
                        {
                                owner->receiver.set_error( e );
                        }

                        void set_stopped()
                        {
                        }
                };

                using sub_sender_t = std::invoke_result_t< CB& >;
                using sub_op_t     = ecor::connect_type< sub_sender_t, sub_receiver >;

                enum class phase : uint8_t
                {
                        rewind,
                        hold
                };

                R                         receiver;
                ecor::task_core*          core;
                core::core*               pcor;
                drv::clk_iface*           piclk;
                drv::pos_iface*           pipos;
                microseconds              end;
                em::min_max< float >      area;
                float                     current;
                CB                        cb;
                gov::curr::_current_gov*  p  = nullptr;
                phase                     ph = phase::rewind;
                std::optional< sub_op_t > sub_op;

                _op( R                    recv,
                     utest&               ctx,
                     core::core&          cor,
                     drv::clk_iface&      iclk,
                     drv::pos_iface&      ipos,
                     microseconds         timeout,
                     em::min_max< float > area_,
                     float                cur,
                     CB                   cb_ )
                  : receiver( std::move( recv ) )
                  , core( &ctx.query( ecor::get_task_core ) )
                  , pcor( &cor )
                  , piclk( &iclk )
                  , pipos( &ipos )
                  , end( iclk.get_us() + timeout )
                  , area( area_ )
                  , current( cur )
                  , cb( std::move( cb_ ) )
                {
                }

                void start()
                {
                        if ( pcor->gov_.activate( "current", pcor->gov_mem ) != status::success ) {
                                receiver.set_error( ASRT_FAILURE );
                                return;
                        }
                        p = dynamic_cast< gov::curr::_current_gov* >( pcor->gov_.active() );
                        if ( !p ) {
                                receiver.set_error( ASRT_FAILURE );
                                return;
                        }
                        core->reschedule( *this );
                }

                void resume() override
                {
                        if ( ph == phase::rewind ) {
                                float pos  = pcor->conv.position.convert( pipos->get_position() );
                                bool  done = em::contains( area, pos ) || piclk->get_us() >= end;
                                if ( done ) {
                                        p->set_goal_current( 0.f );
                                        end = piclk->get_us() + 150_ms;
                                        ph  = phase::hold;
                                        core->reschedule( *this );
                                        return;
                                }
                                float dir = area.max() < pos ? -1.f : 1.f;
                                p->set_goal_current( dir * current );
                        } else if ( piclk->get_us() >= end ) {
                                if ( pcor->gov_.deactivate() != status::success )
                                        receiver.set_error( ASRT_FAILURE );
                                else
                                        receiver.set_value();
                                return;
                        }
                        sub_op.emplace( ecor::connect( cb(), sub_receiver{ this } ) );
                        sub_op->start();
                }
        };

        template < ecor::receiver R >
        auto connect( R receiver ) &&
        {
                return _op< R >{
                    std::move( receiver ),
                    *pctx,
                    *pcor,
                    *piclk,
                    *pipos,
                    timeout,
                    area,
                    current,
                    std::move( cb ) };
        }
};

template < typename CB >
_rewind_sender< CB > rewind(
    utest&               ctx,
    core::core&          cor,
    drv::clk_iface&      iclk,
    drv::pos_iface&      ipos,
    microseconds         timeout,
    em::min_max< float > area,
    float                current,
    CB                   cb )
{
        return { &ctx, &cor, &iclk, &ipos, timeout, area, current, std::move( cb ) };
}

}  // namespace servio::ftest
