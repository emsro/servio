
#include <emlabcpp/algorithm.h>

namespace em = emlabcpp;

namespace servio::ftest::bench
{

struct prof_record
{
        uint32_t                   count = 0;
        std::array< uint16_t, 32 > sum   = em::filled< 32 >( static_cast< uint16_t >( 0 ) );
};

}  // namespace servio::ftest::bench
