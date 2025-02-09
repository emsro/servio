#pragma once

#include "../base.hpp"

#include <cstddef>

namespace servio::paging
{

using state = std::byte;

static constexpr state A = 0x00_b;
static constexpr state B = 0x3E_b;
static constexpr state C = 0x97_b;
static constexpr state D = 0x62_b;

constexpr state next( state x )
{
        state res = B;
        switch ( x ) {
        case A:
                res = B;
                break;
        case B:
                res = C;
                break;
        case C:
                res = D;
                break;
        case D:
                res = B;
                break;
        }
        return res;
}

}  // namespace servio::paging
