
#include <bitset>
#include <cstdint>
#include <span>

namespace servio::cfg
{

struct page_info
{
        page_state st  = page_state::A;
        uint32_t   ver = 0x01;
};

struct record
{
        uint16_t key;
        uint32_t data;
};

using page = std::span< std::byte* >;

}  // namespace servio::cfg
