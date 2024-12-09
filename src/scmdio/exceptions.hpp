
#include <exception>

#pragma once

namespace servio::scmdio
{

struct servio_exception : std::exception
{
};

struct reply_error : servio_exception
{
        reply_error( char const* msg )
          : msg( msg )
        {
        }

        char const* what() const noexcept override
        {
                return msg;
        }

        char const* msg;
};

struct error_exception : servio_exception
{
        error_exception( std::string msg )
          : msg( std::move( msg ) )
        {
        }

        char const* what() const noexcept override
        {
                return msg.c_str();
        }

        std::string msg;
};

struct serialize_error : servio_exception
{
        serialize_error( char const* msg )
          : msg( msg )
        {
        }

        char const* what() const noexcept override
        {
                return msg;
        }

        char const* msg;
};

struct parse_error : servio_exception
{
        parse_error( char const* msg )
          : msg( msg )
        {
        }

        char const* what() const noexcept override
        {
                return msg;
        }

        char const* msg;
};

}  // namespace servio::scmdio
