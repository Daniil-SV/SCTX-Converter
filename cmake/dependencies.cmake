include(FetchContent)

set(WK_SERIALIZATION_JSON ON)
set(WK_CLI_DEPENDENCIES ON)

# install supercell-texture
FetchContent_Declare(
    supercell-texture
    GIT_REPOSITORY https://github.com/sc-workshop/SupercellTexture
    GIT_TAG main
)

FetchContent_MakeAvailable(supercell-texture)