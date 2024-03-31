
#include <emlabcpp/algorithm.h>

namespace em = emlabcpp;

namespace servio::ftest::bench
{

struct prof_record
{
        uint32_t                   count = 0;
        std::array< uint16_t, 64 > max   = { 0 };
        std::array< uint32_t, 64 > sum   = { 0 };
        std::array< uint16_t, 64 > min = em::filled< 64 >( std::numeric_limits< uint16_t >::max() );
};

}  // namespace servio::ftest::bench
