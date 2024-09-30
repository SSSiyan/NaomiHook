set(MODS_SRC
    mods/HpColor.hpp
    mods/HpColor.cpp
    mods/BeamColor.hpp
    mods/BeamColor.cpp
    mods/SimpleMod.hpp
    mods/SimpleMod.cpp
)

set(HOOKS_SRC
    D3D11Hook.hpp
    D3D11Hook.cpp
    WindowsMessageHook.hpp
    WindowsMessageHook.cpp
)

set(SDK_SRC
    sdk/ReClass.hpp
    sdk/ReClass_Internal.hpp
    sdk/Enums_Internal.hpp
    sdk/Offsets.hpp
    sdk/NoMoreHeroesPdb.hpp
)

set(UTILITY_SRC
  utility/StackTrace.hpp
  utility/ExceptionHandler.hpp
  utility/ExceptionHandler.cpp
  utility/ChronoAlias.hpp
  utility/Thread.hpp
  utility/Thread.cpp
  utility/Timer.hpp
  utility/Timer.cpp
  utility/Hash.hpp
  utility/Config.hpp
  utility/Config.cpp
  utility/String.cpp
  utility/String.hpp
  utility/RingBuffer.hpp
  utility/Address.hpp
  utility/Address.cpp
  utility/FunctionHook.hpp
  utility/FunctionHook.cpp
  utility/Memory.hpp
  utility/Memory.cpp
  utility/Module.hpp
  utility/Module.cpp
  utility/Patch.hpp
  utility/Patch.cpp
  utility/Pattern.hpp
  utility/Pattern.cpp
  utility/Scan.hpp
  utility/Scan.cpp
  utility/Input.hpp
  utility/Input.cpp
  utility/Compressed.hpp
  utility/Compressed.cpp
  utility/TextureAtlas.hpp
  utility/TextureAtlas.cpp
)


set(IMGUI_SRC
    fw-imgui/FontFancy.cpp
    fw-imgui/FontInfoblock.cpp
    fw-imgui/FontMain.cpp
    fw-imgui/imgui_impl_win32.h
    fw-imgui/imgui_impl_win32.cpp
    fw-imgui/imgui_impl_dx11.h
    fw-imgui/imgui_impl_dx11.cpp
)

set(FRAMEWORK_SRC
    Mod.hpp
    Mod.cpp
    Mods.hpp
    Mods.cpp
    ModFramework.hpp
    ModFramework.cpp
    GuiFunctions.hpp
    GuiFunctions.cpp
    Fonts.hpp
    Fonts.cpp
    Config.hpp
)

source_group("imgui" FILES ${IMGUI_SRC})
source_group("Hooks" FILES ${HOOKS_SRC})
source_group("Mods" FILES ${MODS_SRC})
source_group("SDK" FILES ${SDK_SRC})
source_group("Utility" FILES ${UTILITY_SRC})
source_group("Framework" FILES ${FRAMEWORK_SRC})

string(REGEX REPLACE "/W0" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_library(${CMAKE_PROJECT_NAME} SHARED
            Main.cpp
            LicenseStrings.hpp
            ${SDK_SRC}
            ${MODS_SRC}
            ${HOOKS_SRC}
            ${UTILITY_SRC}
            ${IMGUI_SRC}
            ${FRAMEWORK_SRC}
)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC imgui minhook d3d11 glm shlwapi)
target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC /GS- /GR-)
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)

include_directories(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME xinput1_4)

if(DEVELOPER_MODE)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC DEVELOPER)
    target_compile_options(${CMAKE_PROJECT_NAME} PUBLIC /EHa)
endif()