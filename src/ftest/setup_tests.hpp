#pragma once

#include "core/core.hpp"
#include "core/drivers.hpp"

#include <emlabcpp/experimental/testing/collect.h>
#include <emlabcpp/experimental/testing/parameters.h>
#include <emlabcpp/experimental/testing/reactor.h>

namespace em = emlabcpp;

namespace servio::ftest
{

void setup_tests(
    em::pmr::memory_resource& mem,
    em::testing::reactor&     reac,
    em::testing::collector&   coll,
    em::testing::parameters&  params,
    core::drivers&            cdrv,
    core::core&               core,
    em::result&               res );

};
