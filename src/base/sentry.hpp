#pragma once

#include "base/status.hpp"

namespace servio::base
{

class central_sentry
{
public:
private:
};

class sentry
{
        // design invariant:
        // once this is set inoperable it can't never recover
public:
        sentry( central_sentry& central )
          : central_( central )
        {
        }

        void set_inoperable()
        {
                is_inoperable_ = true;
        }

        status get_status() const
        {
                if ( is_inoperable_ )
                        return status::INOPERABLE;
                return status::NOMINAL;
        }

private:
        central_sentry& central_;

        bool is_inoperable_ = false;
};

}  // namespace servio::base
