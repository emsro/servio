
#include "./position.hpp"

namespace servio::gov::pos
{

_position_gov           position_gnvr;
static governor_autoreg position_gov_autoreg{ position_gnvr };

}  // namespace servio::gov::pos
