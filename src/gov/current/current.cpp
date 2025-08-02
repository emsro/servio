
#include "./current.hpp"

namespace servio::gov::curr
{

_current_gov            current_gvnr;
static governor_autoreg current_gov_autoreg{ current_gvnr };

}  // namespace servio::gov::curr
