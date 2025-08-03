
#include "./position.hpp"

namespace servio::gov::pos
{

static _position_gov    position_gov;
static governor_autoreg position_gov_autoreg{ position_gov };

}  // namespace servio::gov::pos
