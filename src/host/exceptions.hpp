#include "io.pb.h"

#include <exception>

#pragma once

namespace host
{

struct servio_exception : std::exception
{
};

struct reply_error : servio_exception
{
        reply_error( const char* msg )
          : msg( msg )
        {
        }

        const char* what() const noexcept
        {
                return msg;
        }

        const char* msg;
};

struct error_exception : servio_exception
{
        error_exception( servio::ErrorMsg msg )
          : msg( msg )
        {
        }

        const char* what() const noexcept
        {
                return msg.msg().c_str();
        }

        servio::ErrorMsg msg;
};

struct serialize_error : servio_exception
{
        serialize_error( const char* msg )
          : msg( msg )
        {
        }

        const char* what() const noexcept
        {
                return msg;
        }

        const char* msg;
};

struct parse_error : servio_exception
{
        parse_error( const char* msg )
          : msg( msg )
        {
        }

        const char* what() const noexcept
        {
                return msg;
        }

        const char* msg;
};

}  // namespace host
