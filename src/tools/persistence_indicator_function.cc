/*
  This is a tool shipped by 'Aleph - A Library for Exploring Persistent
  Homology'.

  Given a set of persistence diagrams, it calculates their persistence
  indicator functions and---optionally---their mean indicator function
  if specified by the client.

  All files will be written to '/tmp', prefixed with 'PIF_'.
*/

#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <getopt.h>

#include <aleph/math/KahanSummation.hh>

#include <aleph/persistenceDiagrams/PersistenceDiagram.hh>
#include <aleph/persistenceDiagrams/PersistenceIndicatorFunction.hh>

#include <aleph/persistenceDiagrams/io/Raw.hh>

#include <aleph/utilities/Filesystem.hh>

using DataType           = double;
using PersistenceDiagram = aleph::PersistenceDiagram<DataType>;

int main( int argc, char** argv )
{
  static option commandLineOptions[] =
  {
    { "mean" , no_argument, nullptr, 'm' },
    { nullptr, 0          , nullptr,  0  }
  };

  if( ( argc - optind ) < 1 )
  {
    // TODO: Show usage
    return -1;
  }

  bool calculateMean = false;

  {
    int option = 0;
    while( ( option = getopt_long( argc, argv, "m", commandLineOptions, nullptr ) ) != -1 )
    {
      switch( option )
      {
      case 'm':
        calculateMean = true;
        break;
      default:
        break;
      }
    }
  }

  // Get filenames -----------------------------------------------------

  std::vector<std::string> filenames;
  filenames.reserve( std::size_t( argc - optind ) );

  for( int i = optind; i < argc; i++ )
    filenames.push_back( argv[i] );

  // Load persistence diagrams -----------------------------------------

  std::vector<PersistenceDiagram> persistenceDiagrams;
  persistenceDiagrams.reserve( filenames.size() );

  for( auto&& filename : filenames )
  {
    std::cerr << "* Processing '" << filename << "'...";

    PersistenceDiagram persistenceDiagram = aleph::io::load<DataType>( filename );

    // FIXME: This is only required in order to ensure that the
    // persistence indicator function has a finite integral; it
    // can be solved more elegantly by using a special value to
    // indicate infinite intervals.
    persistenceDiagram.removeUnpaired();

    persistenceDiagrams.push_back( persistenceDiagram );

    std::cerr << "finished\n";
  }

  // Calculate persistence indicator functions -------------------------

  using PersistenceIndicatorFunction = decltype( aleph::persistenceIndicatorFunction( PersistenceDiagram() ) );

  std::vector<PersistenceIndicatorFunction> persistenceIndicatorFunctions;
  persistenceIndicatorFunctions.reserve( persistenceDiagrams.size() );

  PersistenceIndicatorFunction mean;

  unsigned i = 0;
  for( auto&& D : persistenceDiagrams )
  {
    auto f        = aleph::persistenceIndicatorFunction( D );
    auto filename = filenames.at(i);

    if( calculateMean )
      mean += f;

    using namespace aleph::utilities;

    auto outputFilename = "/tmp/PIF_" + stem( basename( filename ) ) + ".txt";

    std::cerr << "* Writing persistence indicator function to '" << outputFilename << "'...\n";

    std::ofstream out( outputFilename );
    out << f << "\n";

    persistenceIndicatorFunctions.emplace_back( f );

    ++i;
  }

  if( calculateMean )
  {
    mean                /= static_cast<DataType>( persistenceDiagrams.size() );
    auto outputFilename  = "/tmp/PIF_mean.txt";

    std::cerr << "* Writing mean persistence indicator function to '" << outputFilename << "'...\n";

    std::ofstream out( outputFilename );
    out << mean << "\n";

    // Since Y is supposed to be a random variable at this point, this
    // nomenclature makes sense.
    auto Y = mean.integral();

    std::cerr << "* Norm of the mean persistence indicator function: " << Y << "\n";

    std::vector<double> squaredDifferences( persistenceDiagrams.size() );

    std::transform( persistenceIndicatorFunctions.begin(), persistenceIndicatorFunctions.end(),
                    squaredDifferences.begin(),
                      [&Y] ( const PersistenceIndicatorFunction& f )
                      {
                        auto Z = f.integral();
                        return (Z-Y) * (Z-Y);
                      }
                    );

    auto s
      = aleph::math::accumulate_kahan_sorted( squaredDifferences.begin(),
                                              squaredDifferences.end(),
                                              0.0 );

    if( persistenceDiagrams.size() > 1 )
      s /= static_cast<double>( persistenceDiagrams.size() - 1 );
    else
      s = std::numeric_limits<double>::infinity();

    std::cerr << "* Sample variance: " << s << "\n";
  }
}
