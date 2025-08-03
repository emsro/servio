
#include "./current.hpp"

namespace servio::gov::curr
{

static _current_gov     current_gov;
static governor_autoreg current_gov_autoreg{ current_gov };

}  // namespace servio::gov::curr
