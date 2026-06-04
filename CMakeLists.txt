cmake_minimum_required(VERSION 3.16)
project(CrossPlatformQtGame LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Test)

qt_standard_project_setup()

qt_add_executable(CrossPlatformQtGame
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.h mainwindow.cpp
    menuwidget.h menuwidget.cpp
    gamewidget.h gamewidget.cpp
    busitem.h busitem.cpp
    selectnivelwidget.h selectnivelwidget.cpp
    passengeritem.h passengeritem.cpp
    gamepersistence.h gamepersistence.cpp
    levelloader.h levelloader.cpp
    parkingarea.h parkingarea.cpp
    gameconfig.h
    passengerqueue.h passengerqueue.cpp
    autosolveria.h autosolveria.cpp
    estadojogo.h
)

qt_add_executable(Testes
    tst_autosolveria.cpp autosolveria.cpp
)

target_link_libraries(CrossPlatformQtGame
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Test
        Qt6::Widgets
)

target_link_libraries(Testes
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Test
)

include(GNUInstallDirs)
install(TARGETS CrossPlatformQtGame
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET CrossPlatformQtGame
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
