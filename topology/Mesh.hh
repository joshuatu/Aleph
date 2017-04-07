#ifndef ALEPH_TOPOLOGY_MESH_HH__
#define ALEPH_TOPOLOGY_MESH_HH__

#include <cassert>

#include <algorithm>
#include <iterator>
#include <memory>
#include <unordered_set>
#include <vector>

namespace aleph
{

namespace topology
{

/**
  @class Mesh
  @brief Half-edge mesh data structure

  This data structure is capable of representing two-dimensional piecewise
  linear manifolds. In order to speed up standard queries, this class uses
  a standard half-edge data structure.
*/

template <class Position = float, class Data = float> class Mesh
{
public:
  struct Face;
  struct HalfEdge;
  struct Vertex;

  using Index           = std::size_t;

  using FacePointer     = std::shared_ptr<Face>;
  using HalfEdgePointer = std::shared_ptr<HalfEdge>;
  using VertexPointer   = std::shared_ptr<Vertex>;

  struct HalfEdge
  {
    FacePointer   face;
    VertexPointer vertex;

    HalfEdgePointer next; // Next half-edge (counter-clockwise)
    HalfEdgePointer prev; // Previous half-edge
    HalfEdgePointer pair; // Opposite half-edge

    VertexPointer source() const noexcept
    {
      return pair->vertex;
    }

    VertexPointer target() const noexcept
    {
      return vertex;
    }
  };

  struct Face
  {
    HalfEdgePointer edge;
  };

  struct Vertex
  {
    Index    id   = Index();
    Position x    = Position();
    Position y    = Position();
    Position z    = Position();
    Data     data = Data();

    HalfEdgePointer edge;
  };

  // Mesh attributes ---------------------------------------------------

  std::size_t vertices() const noexcept
  {
    return _vertices.size();
  }

  std::size_t faces() const noexcept
  {
    std::unordered_set<FacePointer> faces;

    for( auto&& vertex : _vertices )
    {
      auto&& edge = vertex->edge;
      auto&& face = edge->face;

      if( face )
        faces.insert( face );
    }

    return faces.size();
  }

  // Mesh modification -------------------------------------------------

  /** Adds a new vertex to the mesh */
  void addVertex( Position x, Position y, Position z, Data data = Data() )
  {
    Vertex v;

    v.id   = _vertices.size();
    v.x    = x;
    v.y    = y;
    v.z    = z;
    v.data = data;

    _vertices.push_back( std::make_shared<Vertex>( v ) );
  }

  /**
    Adds a new face to the mesh. This function expects a range of vertex IDs
    that make up the face. The vertices of the face need to sorted correctly
    in order for the orientation to be consistent.
  */

  template <class InputIterator> void addFace( InputIterator begin, InputIterator end )
  {
    FacePointer face = std::make_shared<Face>();

    // Stores all half-edges created (or found) by this function in the
    // order in which they belong to the face.
    std::vector<HalfEdgePointer> edges;
    edges.reserve( static_cast<std::size_t>( std::distance( begin, end ) ) );

    for( InputIterator it = begin; it != end; ++it )
    {
      auto curr = it;
      auto next = std::next( it );

      if( next == end )
        next = begin;

      auto source = _vertices.at( *curr );   // Edge source vertex
      auto target = _vertices.at( *next );   // Edge target vertex
      auto edge   = getEdge( *curr, *next ); // Edge

      // Case 1: A new edge. Create a new edge and a new pair. Set edges
      // of source and target vertex correctly. Moreover, initialize the
      // paired edge with sensible default values.
      if( !edge )
      {
        edge      = std::make_shared<HalfEdge>();
        auto pair = std::make_shared<HalfEdge>();

        pair->vertex = source; // This is flipped by design: We point to the
                               // target vertex of the flipped edge. This is
                               // just the source vertex again.

        pair->pair   = edge;
        edge->pair   = pair;

        if( !source->edge )
          source->edge = edge;

        if( !target->edge )
          target->edge = pair;
      }

      assert( !edge->face );
      assert(  edge->pair );

      edge->face   = face;
      edge->vertex = target;

      edges.push_back( edge );
    }

    // Set 'next' and 'prev' pointers correctly ------------------------
    //
    // We first traverse all edges that bound the current face. Here, it
    // should be possible to traverse the face directly, so we require a
    // proper pointer in both directions.

    for( auto itEdge = edges.begin(); itEdge != edges.end(); ++itEdge )
    {
      auto curr = itEdge;
      auto prev = std::prev( curr );
      auto next = std::next( curr );

      if( curr == edges.begin() )
        prev = std::prev( edges.end() );

      if( next == edges.end() )
        next = edges.begin();

      auto&& edge = *itEdge;

      //assert( !edge->next );
      //assert( !edge->prev );

      edge->next = *next;
      edge->prev = *prev;
    }

    // Extend boundary -------------------------------------------------
    //
    // Traverse all vertices whose paired edges have an empty face. Any
    // of these edges is part of the boundary face.

    for( auto&& vertex : _vertices )
    {
      if( !vertex->edge || vertex->edge->pair->face )
        continue;

      auto curr = vertex->edge->target();
      auto edge = vertex->edge->pair;

      do
      {
        assert( !edge->face );
        auto edges = this->getEdges( *curr );

        for( auto&& e : edges )
        {
          if( !e->pair->face )
          {
            e->pair->next = edge;

            edge          = e->pair;
            curr          = e->pair->pair->target();

            break;
          }
        }
      }
      while( curr != vertex );

      // Close the loop around the boundary face by adding a pointer to
      // the identified edge.
      vertex->edge->pair->next = edge;
    }
  }

  // Mesh queries ------------------------------------------------------

  /**
    The closed star of a vertex is defined as the smallest simplicial
    subcomplex containing the given vertex and all simplices of which
    the vertex is a face.
  */

  Mesh closedStar( const Vertex& v ) const noexcept
  {
    auto faces = this->getFaces( v );

    // TODO:
    //   - Get all faces
    //   - Enumerate their vertices
    //   - Add them to the new mesh
    //   - Add the faces to the new mesh

    return {};
  }

  /**
    The link of a vertex is defined as all simplices in the closed star
    that are disjoint from the vertex. For 2-manifolds, this will yield
    a cycle of edges and vertices.

    This function will represent the cycle by returning all vertex IDs,
    in an order that is consistent with the orientation of the mesh.
  */

  std::vector<Index> link( const Vertex& v ) const noexcept
  {
    auto neighbours = this->getNeighbours( v );

    std::vector<Index> result;
    result.reserve( neighbours.size() );

    for( auto&& neighbour : neighbours )
      result.push_back( neighbour->id );

    return result;
  }

  std::vector<VertexPointer> getLowerNeighbours( const Vertex& v ) const noexcept
  {
    auto neighbours = this->getNeighbours( v );
    auto data       = v.data;

    neighbours.erase( std::remove_if( neighbours.begin(), neighbours.end(),
                                      [&data] ( const VertexPointer& neighbour )
                                      {
                                        return neighbour->data >= data;
                                      } ),
                      neighbours.end() );

    return neighbours;
  }

  std::vector<VertexPointer> getHigherNeighbours( const Vertex& v ) const noexcept
  {
    auto neighbours = this->getNeighbours( v );
    auto data       = v.data;

    neighbours.erase( std::remove_if( neighbours.begin(), neighbours.end(),
                                      [&data] ( const VertexPointer& neighbour )
                                      {
                                        return neighbour->data <= data;
                                      } ),
                      neighbours.end() );

    return neighbours;
  }

  VertexPointer vertex( Index id ) const
  {
    // TODO: Improve ID-based query? Currently, ID and index of the
    // vertex with respect to the vector are the same. I am not sure
    // whether this is smart.
    return _vertices.at( id );
  }

  /**
    Checks whether an edge between two vertices that are identified by
    their index exists.
  */

  bool hasEdge( Index u, Index v ) const
  {
    auto&& source = this->vertex(u);
    auto&& target = this->vertex(v);

    auto neighbours = this->getNeighbours( *source );

    return std::find( neighbours.begin(), neighbours.end(), target ) != neighbours.end();
  }

private:

  /** Gets all vertices that are adjacent to a given vertex */
  std::vector<VertexPointer> getNeighbours( const Vertex& v ) const noexcept
  {
    std::vector<VertexPointer> neighbours;

    auto edge = v.edge;
    do
    {
      if( edge )
        neighbours.push_back( edge->target() );
      else
        break;

      edge = edge->pair->next;
    }
    while( edge != v.edge );

    return neighbours;
  }

  /** Gets all edges that are incident on a given vertex. */
  std::vector<HalfEdgePointer> getEdges( const Vertex& v ) const noexcept
  {
    std::vector<HalfEdgePointer> edges;

    auto edge = v.edge;
    do
    {
      if( edge )
        edges.push_back( edge );
      else
        break;

      edge = edge->pair->next;
    }
    while( edge != v.edge );

    return edges;
  }

  /** Gets all faces that are incident on a given vertex */
  std::vector<FacePointer> getFaces( const Vertex& v ) const noexcept
  {
    std::vector<FacePointer> faces;

    auto edge = v.edge;
    do
    {
      if( edge && edge->face )
        faces.push_back( edge->face );
      else
        break;

      edge = edge->pair->next;
    }
    while( edge != v.edge );

    return faces;
  }

  /**
    Check whether a given (directed) edge already exists. If so,
    a pointer to the edge is being returned.
  */

  HalfEdgePointer getEdge( Index u, Index v ) const noexcept
  {
    auto source = _vertices.at(u);           // Edge source vertex
    auto target = _vertices.at(v);           // Edge target vertex
    auto edges  = this->getEdges( *source ); // Incident edges

    auto itEdge = std::find_if( edges.begin(), edges.end(),
                                [&source, &target] ( const HalfEdgePointer& edge )
                                {
                                  return edge->source() == source && edge->target() == target;
                                } );

    if( itEdge != edges.end() )
      return *itEdge;
    else
      return nullptr;
  }

  /**
    Stores all vertex pointers. This is sufficient to store the
    complete mesh.
  */

  std::vector<VertexPointer> _vertices;
};

} // namespace topology

} // namespace aleph

#endif