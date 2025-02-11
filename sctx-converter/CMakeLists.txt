# Project: supercell-flash sctx-converter

include(WkJson)
include(WkArgsparse)

# C++ headers and source files
file(GLOB_RECURSE HEADERS source/*.h)
file(GLOB_RECURSE SOURCES source/*.cpp source/*.hpp)

set(CLI_TARGET SctxConverter)

add_executable(${CLI_TARGET} ${HEADERS} ${SOURCES})

set_target_properties(${CLI_TARGET} PROPERTIES
    FOLDER SupercellSDK/tools
)

target_include_directories(${CLI_TARGET} PUBLIC source)

target_link_libraries(${CLI_TARGET} PRIVATE supercell::texture)

wk_project_setup(${CLI_TARGET})
wk_include_argparse()
wk_include_json()

