
#include "./simulate.hpp"

namespace servio::klmn
{

std::vector< state > simulate(
    sec_time                          tdiff,
    float                             process_deviation,
    float                             observation_deviation,
    std::vector< observation > const& observations,
    state_range                       sr )
{
        state_transition_model const   f = get_transition_model( tdiff );
        control_input_model const      b = get_control_input_model( tdiff );
        process_noise_covariance const q = get_process_noise_covariance( tdiff, process_deviation );
        observation_model const        h = get_observation_model();
        observation_noise_covariance const r =
            get_observation_noise_covariance( observation_deviation );

        state x;
        angle( x )    = angle( observations[0] );
        velocity( x ) = 0.F;
        state_covariance p;

        control_input u;
        angle( u ) = 0.F;

        std::vector< state > res;

        float offset = 0.F;
        for ( observation z : observations ) {
                modify_angle( z, +offset, sr );

                std::tie( x, p ) = predict( x, p, u, f, b, q );
                std::tie( x, p ) = update( x, p, z, h, r );

                if ( requires_offset( angle( x ), sr ) ) {
                        offset = angle_mod( offset + pi, sr );
                        modify_angle( x, +pi, sr );
                }

                res.push_back( x );
                modify_angle( res.back(), -offset, sr );
        }

        return res;
}

}  // namespace servio::klmn
