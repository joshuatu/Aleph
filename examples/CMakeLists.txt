IF( BUILD_EXAMPLES )
  MESSAGE( STATUS "Building examples" )

  ADD_EXECUTABLE( 1D                               1D.cc                               )
  ADD_EXECUTABLE( create_persistence_diagrams      create_persistence_diagrams.cc      )
  ADD_EXECUTABLE( create_random_graph              create_random_graph.cc              )
  ADD_EXECUTABLE( image_analysis                   image_analysis.cc                   )
  ADD_EXECUTABLE( network_analysis                 network_analysis.cc                 )
  ADD_EXECUTABLE( ply                              ply.cc                              )
  ADD_EXECUTABLE( vtk                              vtk.cc                              )
  ADD_EXECUTABLE( vietoris_rips                    vietoris_rips.cc                    )
  ADD_EXECUTABLE( vietoris_rips_eccentricity       vietoris_rips_eccentricity.cc       )
  ADD_EXECUTABLE( witness_complex                  witness_complex.cc                  )

  ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-O3" )
ELSE()
  MESSAGE( STATUS "Not building examples (toggle BUILD_EXAMPLES to change this)" )
ENDIF()
