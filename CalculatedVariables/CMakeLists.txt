include_directories(ext_components/muparser-amalgamated/include)

file(GLOB muparser_srcs "ext_components/muparser-amalgamated/src/*.cpp" )

add_definitions(-DMUPARSER_STATIC)

add_library( 
    CalculatedVariables OBJECT  
    src/CalculatedVariable.cpp
    src/CalculatedVariablesEngine.cpp
    src/CalculatedVariablesChangeListener.cpp
    src/ParserVariable.cpp
    ${muparser_srcs} 
)

add_dependencies( CalculatedVariables Configuration.hxx_GENERATED )