cmake_minimum_required(VERSION 3.8)

 # set version number
 set (OPENTYRIAN_VERSION 2.1)

 # set the project name
 project(OpenTyrian VERSION ${OPENTYRIAN_VERSION})

 set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

 # Options enabled by default
 option ( enable-network "compile support for network" off )

 # Include required scripts
 include(GNUInstallDirs)

 set(USE_SDL_VERSION SDL2)

 find_package(${USE_SDL_VERSION} REQUIRED)

 if (enable-network)
     find_package(SDL2_net REQUIRED)
     add_definitions(-DWITH_NETWORK)
 endif()

 # add the executable
 add_executable(opentyrian WIN32
     src/animlib.c
     src/arg_parse.c
     src/backgrnd.c
     src/config.c
     src/config_file.c
     src/destruct.c
     src/editship.c
     src/episodes.c
     src/file.c
     src/font.c
     src/fonthand.c
     src/game_menu.c
     src/helptext.c
     src/joystick.c
     src/jukebox.c
     src/keyboard.c
     src/lds_play.c
     src/loudness.c
     src/lvllib.c
     src/lvlmast.c
     src/mainint.c
     src/menus.c
     src/mouse.c
     src/mtrand.c
     src/musmast.c
     src/network.c
     src/nortsong.c
     src/nortvars.c
     src/opentyr.c
     src/opl.c
     src/palette.c
     src/params.c
     src/pcxload.c
     src/pcxmast.c
     src/picload.c
     src/player.c
     src/shots.c
     src/sizebuf.c
     src/sndmast.c
     src/sprite.c
     src/starlib.c
     src/tyrian2.c
     src/varz.c
     src/vga256d.c
     src/vga_palette.c
     src/video.c
     src/video_scale.c
     src/video_scale_hqNx.c
     src/xmas.c
 )

target_compile_definitions(opentyrian PUBLIC
    TARGET_UNIX
    TYRIAN_DIR="/usr/share/opentyrian/data/"
)

 # Setup version
 target_compile_definitions(opentyrian PUBLIC
     OPENTYRIAN_VERSION=\"${OPENTYRIAN_VERSION}\"
 )

 # Compile options
 target_include_directories(opentyrian
     PRIVATE
     ${CMAKE_SOURCE_DIR}/src
     ${SDL2_INCLUDE_DIRS}
 )

target_link_libraries(opentyrian ${SDL2_LIBRARIES})

 find_library(MATH_LIBRARY m)

 if (MATH_LIBRARY)
     message(STATUS "Add math library ${MATH_LIBRARY}")
     target_link_libraries(opentyrian
         ${MATH_LIBRARY}
     )
 endif()

#file(GLOB GAME_RESOURCES_FILES "/data/**")
#file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data)
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

