#include <args.hxx>
#include <emlabcpp/view.h>

#pragma once

namespace em = emlabcpp;

namespace host
{

inline const std::string*
parse_args( args::ArgumentParser& parser, em::view< const std::string* > args )
{
        try {
                return parser.ParseArgs( args );
        }
        catch ( const args::Completion& e ) {
                std::cout << e.what();
                return args.end();
        }
        catch ( const args::Help& ) {
                std::cout << parser;
                return args.end();
        }
        catch ( const args::ParseError& e ) {
                std::cerr << e.what() << std::endl;
                std::cerr << parser;
                return nullptr;
        }
}

inline int parse_cli( args::ArgumentParser& parser, int argc, char* argv[] )
{
        std::vector< std::string > vec( argv + 1, argv + argc );
        parser.Prog( argv[0] );
        if ( parse_args( parser, em::data_view( vec ) ) == nullptr ) {
                return 1;
        }
        return 0;
}

}  // namespace host
