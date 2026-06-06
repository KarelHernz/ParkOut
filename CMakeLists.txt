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

set_target_properties(CrossPlatformQtGame PROPERTIES
   QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")

qt_add_executable(Testes
    testes.cpp
    autosolveria.cpp
    gamepersistence.cpp
    busitem.cpp
    parkingarea.cpp
    passengerqueue.cpp
    passengeritem.cpp
)

qt_add_resources(CrossPlatformQtGame "Recursos"
    PREFIX "/"
    FILES
        src/json/levels.json
        src/json/progress.json
        src/json/recordes.json
        src/img/ParkOut_Logo.png
)

target_link_libraries(CrossPlatformQtGame
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Test
        Qt::Widgets
)

target_link_libraries(Testes
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Test
        Qt::Widgets
)

if(EMSCRIPTEN)
    target_link_options(CrossPlatformQtGame PRIVATE
        "-s" "USE_PTHREADS=1"
        "-s" "PTHREAD_POOL_SIZE=2"
    )

    target_link_options(Testes PRIVATE
        "-s" "USE_PTHREADS=1"
    )
endif()

if(ANDROID)
    #Define a pasta onde está o AndroidManifest.xml
    set_property(TARGET CrossPlatformQtGame PROPERTY
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
    set_property(TARGET CrossPlatformQtGame PROPERTY
        QT_ANDROID_PACKAGE_NAME "com.parkout.game"
    )

    set_property(TARGET CrossPlatformQtGame PROPERTY OUTPUT_NAME "ParkOut")
endif()

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
