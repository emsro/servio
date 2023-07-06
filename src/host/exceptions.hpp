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

        virtual const char* what() const noexcept
        {
                return msg;
        }

        const char* msg;
};

struct status_error : servio_exception
{
        virtual const char* what() const noexcept
        {
                return status_string.c_str();
        }

        status_error( servio::Status status )
          : status_string( servio::Status_Name( status ) )
        {
        }

        std::string status_string;
};

struct serialize_error : servio_exception
{
        serialize_error( const char* msg )
          : msg( msg )
        {
        }

        virtual const char* what() const noexcept
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

        virtual const char* what() const noexcept
        {
                return msg;
        }

        const char* msg;
};

}  // namespace host
