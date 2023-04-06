
namespace ctl
{

struct linear_transition_regulator
{
        float low_point  = 1.f;
        float high_point = 2.f;

        float        decay = 1.f;
        microseconds last_time;

        float state = low_point;

        void set_config( float scale, float dec )
        {
                high_point = scale;
                decay      = dec / 1'000'000.f;  // input decay is /s, this makes it /us
        }

        void update( microseconds now, bool is_moving )
        {
                auto  tdiff = now - last_time;
                float dir   = is_moving ? -1.f : 1.f;
                float step  = decay * static_cast< float >( tdiff.count() );

                state     = std::clamp( state + dir * step, low_point, high_point );
                last_time = now;
        }
};

}  // namespace ctl
