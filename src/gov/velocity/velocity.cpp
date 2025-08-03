
#include "./velocity.hpp"

namespace servio::gov::vel
{

static _velocity_gov    velocity_gov;
static governor_autoreg velocity_gov_autoreg{ velocity_gov };

}  // namespace servio::gov::vel
