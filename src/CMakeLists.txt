set(MODS_SRC
    mods/BrainAge.hpp
    mods/BrainAge.cpp
    mods/FreeCam.hpp
    mods/FreeCam.cpp
    mods/HudSettings.hpp
    mods/HudSettings.cpp
    mods/KbmControls.hpp 
    mods/KbmControls.cpp 
    mods/Tony.hpp
    mods/Tony.cpp
    mods/HpColor.hpp
    mods/HpColor.cpp
    mods/BeamColor.hpp
    mods/BeamColor.cpp
    mods/SimpleMod.hpp
    mods/SimpleMod.cpp
    mods/NMHFixes.hpp
    mods/NMHFixes.cpp
    mods/AcceptPadInputsTabbedOut.hpp
    mods/AcceptPadInputsTabbedOut.cpp
    mods/StageWarp.hpp
    mods/StageWarp.cpp
    mods/PlayerTracker.hpp
    mods/PlayerTracker.cpp
    mods/WeaponSwitcher.hpp 
    mods/WeaponSwitcher.cpp 
    mods/ClothesSwitcher.hpp 
    mods/ClothesSwitcher.cpp 
    mods/LockOnSettings.hpp 
    mods/LockOnSettings.cpp 
    mods/HitstopSettings.hpp 
    mods/HitstopSettings.cpp 
    mods/CameraSettings.hpp 
    mods/CameraSettings.cpp 
    mods/DodgeSettings.hpp 
    mods/DodgeSettings.cpp 
    mods/SprintSettings.hpp
    mods/SprintSettings.cpp
    mods/SwordColours.hpp 
    mods/SwordColours.cpp 
    mods/ThrowBattery.hpp 
    mods/ThrowBattery.cpp 
    mods/ChargeSubsBattery.hpp 
    mods/ChargeSubsBattery.cpp 
    mods/DebugText.hpp 
    mods/DebugText.cpp 
    mods/ReprisalSwap.hpp 
    mods/ReprisalSwap.cpp 
    mods/DisableMouse.hpp 
    mods/DisableMouse.cpp 
    mods/SoundTracker.hpp 
    mods/SoundTracker.cpp 
    mods/ArcadeMode.hpp 
    mods/ArcadeMode.cpp 
    mods/QuickBoot.hpp 
    mods/QuickBoot.cpp 
    mods/EnemyTracker.hpp 
    mods/EnemyTracker.cpp 
    mods/StanceControl.hpp 
    mods/StanceControl.cpp 
    mods/EnemySpawn.hpp 
    mods/EnemySpawn.cpp 
    mods/GameplayFixes.hpp 
    mods/GameplayFixes.cpp 
    mods/Cheats.hpp 
    mods/Cheats.cpp 
    mods/KillObscuringTreasureChests.hpp
    mods/KillObscuringTreasureChests.cpp
    mods/Volume.hpp 
    mods/Volume.cpp 
    mods/ResolutionScaleFix.hpp  
    mods/ResolutionScaleFix.cpp
    mods/ShaderEdit.hpp
    mods/ShaderEdit.cpp
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
    sdk/nmh.hpp
    sdk/nmh.cpp
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
    fw-imgui/SwordsTextureAtlas.cpp
    fw-imgui/SwordSwitchAtlas.cpp
    fw-imgui/StageWarpAtlas.cpp
    fw-imgui/KanaeTextureAtlas.cpp
    fw-imgui/FontFancy.cpp
    fw-imgui/FontInfoblock.cpp
    fw-imgui/FontMain.cpp
    fw-imgui/FontTony.cpp
    fw-imgui/Texture2DD3D11.hpp
    fw-imgui/Texture2DD3D11.cpp
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
    ImageViewer.hpp
    ImageViewer.cpp
    Config.hpp
)

source_group("imgui" FILES ${IMGUI_SRC})
source_group("Hooks" FILES ${HOOKS_SRC})
source_group("Mods" FILES ${MODS_SRC})
source_group("SDK" FILES ${SDK_SRC})
source_group("Utility" FILES ${UTILITY_SRC})
source_group("Framework" FILES ${FRAMEWORK_SRC})

#string(REGEX REPLACE "/W0" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

macro(build_my_lib target_name def output_name extension)
  add_library(${target_name} SHARED Main.cpp LicenseStrings.hpp ${SDK_SRC} ${MODS_SRC} ${HOOKS_SRC} ${UTILITY_SRC} ${IMGUI_SRC} ${FRAMEWORK_SRC})
  target_link_libraries(${target_name} PUBLIC imgui minhook d3d11 glm csys::csys shlwapi)
  target_compile_definitions(${target_name} PRIVATE ${def})
  include_directories(${target_name} ${CMAKE_CURRENT_SOURCE_DIR})
  set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "${output_name}")
  set_target_properties(${target_name} PROPERTIES SUFFIX "${extension}")
endmacro()

# windows dll with fancy proxying tech that doesn't work on wine
build_my_lib(${CMAKE_PROJECT_NAME} NMH_WINDOWS xinput1_4 .dll)

# build .asi file for use on Linux with ASI injector
if(BUILD_ASI)
  build_my_lib(NaomiHook_Linux NMH_LINUX NaomiHook_Linux .asi)
endif()

