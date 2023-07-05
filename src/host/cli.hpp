#include <args.hxx>

#pragma once

namespace host
{

inline int parse_cli( args::ArgumentParser& parser, int argc, char* argv[] )
{
        try {
                parser.ParseCLI( argc, argv );
        }
        catch ( const args::Completion& e ) {
                std::cout << e.what();
                return 0;
        }
        catch ( const args::Help& ) {
                std::cout << parser;
                return 0;
        }
        catch ( const args::ParseError& e ) {
                std::cerr << e.what() << std::endl;
                std::cerr << parser;
                return 1;
        }
        return 0;
}

}  // namespace host
