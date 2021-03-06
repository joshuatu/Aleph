#include <aleph/config/Eigen.hh>

#include <aleph/geometry/HeatKernel.hh>
#include <aleph/geometry/RipsExpander.hh>

#include <aleph/topology/io/GML.hh>

#include <aleph/topology/Simplex.hh>
#include <aleph/topology/SimplicialComplex.hh>

#include <aleph/topology/filtrations/Data.hh>

#include <aleph/utilities/Filesystem.hh>

#ifdef ALEPH_WITH_EIGEN
  #include <Eigen/Eigenvalues>
#endif

#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

#include <getopt.h>

// Auxiliary class for storing the spectrum of a graph, i.e. the set of
// eigenvalues. This class offers a simple distance calculation.
template <class T> class Spectrum
{
public:
  Spectrum() = default;

  template <class InputIterator> Spectrum( InputIterator begin, InputIterator end )
    : _eigenvalues( begin, end )
  {
  }

  std::size_t size() const noexcept
  {
    return _eigenvalues.size();
  }

  T distance( const Spectrum& other ) const noexcept
  {
    auto n         = this->size();
    auto m         = other.size();
    auto spectrum1 = this->_eigenvalues;
    auto spectrum2 = other._eigenvalues;

    spectrum1.resize( std::max(n,m) );
    spectrum2.resize( std::max(n,m) );

    std::sort( spectrum1.begin(), spectrum1.end(), std::greater<T>() );
    std::sort( spectrum2.begin(), spectrum2.end(), std::greater<T>() );

    // Squared Euclidean distance
    return std::inner_product( spectrum1.begin(), spectrum1.end(),
                               spectrum2.begin(),
                               T(0),
                               std::plus<T>(),
                               []( T x,T y )
                               {
                                 return ( x-y )*( x-y );
                               }
    );
  }

private:
  std::vector<T> _eigenvalues;
};

int main( int argc, char** argv )
{
  std::string nodeAttribute;
  bool reverseFiltration = false;

  {
    static option commandLineOptions[] =
    {
      { "node-attribute"    , required_argument, nullptr, 'n' },
      { "reverse-filtration", no_argument      , nullptr, 'r' },
      { nullptr             , 0                , nullptr,  0  }
    };

    int option = 0;
    while( ( option = getopt_long( argc, argv, "n:", commandLineOptions, nullptr ) ) != -1 )
    {
      switch( option )
      {
      case 'n':
        nodeAttribute = optarg;
        break;

      case 'r':
        reverseFiltration = true;
        break;

      default:
        break;
      }
    }
  }

  if( argc - optind <= 0 )
    return -1;

  using DataType          = double;
  using VertexType        = unsigned short;
  using Simplex           = aleph::topology::Simplex<DataType, VertexType>;
  using SimplicialComplex = aleph::topology::SimplicialComplex<Simplex>;
  using Spectrum          = Spectrum<DataType>;

  std::vector<std::string> filenames;
  std::map<std::string, unsigned> filename_to_id;
  std::map<unsigned, unsigned>    id_to_id;

  for( int i = optind; i < argc; i++ )
  {
    filenames.push_back( argv[i] );

    std::regex reDataSetSuffix( "\\D*([[:digit:]]+).*" );
    std::smatch matches;

    auto basename = aleph::utilities::basename( filenames.back() );

    if( std::regex_match( basename, matches, reDataSetSuffix ) )
    {
      unsigned id                        = static_cast<unsigned>( std::stoull( matches[1] ) );
      filename_to_id[ filenames.back() ] = id;
    }
    else
      throw std::runtime_error( "Unable to identify ID" );
  }

  {
    std::set<unsigned> ids;

    for( auto&& pair : filename_to_id )
      ids.insert( pair.second );

    unsigned i = 0;
    for( auto&& id : ids )
      id_to_id[ id ] = i++;
  }

  aleph::topology::io::GMLReader reader;

  std::vector<Spectrum> spectra( filenames.size() );

  for( auto&& filename : filenames )
  {
    std::cerr << "* Processing '" << filename << "'...";

    SimplicialComplex K;
    reader( filename, K );

    // Node node attribute has been specified, so we fall back to adding
    // uniform weights to all edges.
    if( nodeAttribute.empty() )
    {
      for( auto it = K.begin(); it != K.end(); ++it )
      {
        auto simplex = *it;
        if( simplex.dimension() == 1 )
        {
          simplex.setData( 1 );
          K.replace( it, simplex );
        }
      }

      K.sort();
    }
    else
    {
      auto id_to_index  = reader.id_to_index<VertexType>();
      auto attributeMap = reader.getNodeAttribute( nodeAttribute );

      // Assumption: attribute map contains exactly the same number of
      // entries than the number of vertices in the graph
      std::vector<DataType> data( attributeMap.size() );

      for( auto&& pair : attributeMap )
      {
        bool success = false;
        auto index   = id_to_index[pair.first];
        auto value   = aleph::utilities::convert<DataType>( pair.second, success );

        if( !success )
          throw std::runtime_error( "Unable to convert attribute value to data type" );

        data.at( index ) = value;
      }

      aleph::geometry::RipsExpander<SimplicialComplex> expander;
      K = expander.assignMaximumData( K, data.begin(), data.end() );

      // Recalculate all weights in the simplicial complex based on the
      // specified node attribute.
      bool useMaximum                  = reverseFiltration ? false : true;
      bool skipOneDimensionalSimplices = false;

      K.recalculateWeights( useMaximum, skipOneDimensionalSimplices );
    }

    K.sort(
      aleph::topology::filtrations::Data<Simplex>()
    );

    auto L           = aleph::geometry::weightedLaplacianMatrix( K );
    using MatrixType = decltype(L);

    Eigen::SelfAdjointEigenSolver<MatrixType> solver;
    solver.compute( L );

    auto eigenvalues = solver.eigenvalues();

    {
      std::vector<DataType> data;
      for( decltype( eigenvalues.size() ) i = 0; i < eigenvalues.size(); i++ )
        data.push_back( eigenvalues( i ) );

      spectra.at( id_to_id[ filename_to_id[filename] ] ) = Spectrum( data.begin(), data.end() );
    }

    std::cerr << "finished\n";

    auto numEdges = std::count_if( K.begin(), K.end(),
      [] ( const Simplex& s )
      {
        return s.dimension() == 1;
      }
    );

    std::cerr << "* Extracted graph has " << numEdges << " edges\n";
  }

  // Calculate spectral distances --------------------------------------

  std::vector< std::vector<DataType> > distances( spectra.size(),
    std::vector<DataType>( spectra.size() ) );

  for( std::size_t i = 0; i < spectra.size(); i++ )
  {
    for( std::size_t j = i+1; j < spectra.size(); j++ )
    {
      distances[i][j] = spectra[i].distance( spectra[j] );
      distances[j][i] = distances[i][j];
    }
  }

  for( std::size_t i = 0; i < spectra.size(); i++ )
  {
    for( std::size_t j = 0; j < spectra.size(); j++ )
    {
      if( j != 0 )
        std::cout << " ";

      std::cout << distances[i][j];
    }

    std::cout << "\n";
  }

  std::cout << "\n\n";
}
