
#include "./velocity.hpp"

namespace servio::gov::vel
{

_velocity_gov           velocity_gnvr;
static governor_autoreg velocity_gov_autoreg{ velocity_gnvr };

}  // namespace servio::gov::vel
