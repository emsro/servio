
#include "./current.hpp"

namespace servio::gvnr::curr
{

_current_gov            current_gnvr;
static governor_autoreg current_gov_autoreg{ current_gnvr };

}  // namespace servio::gvnr::curr
