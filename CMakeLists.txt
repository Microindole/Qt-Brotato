cmake_minimum_required(VERSION 3.16)

project(brotato VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(brotato
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        bullet.h bullet.cpp
        enemy.h enemy.cpp
        player.h player.cpp
        menu.ui menu.h menu.cpp
        gamewidget.ui gamewidget.h
        
        # 主游戏逻辑文件
        gamewidget.cpp
        
        # 新拆分出来的文件
        gamewidget_audio.cpp
        gamewidget_collisions.cpp
        gamewidget_events.cpp
        gamewidget_state.cpp
        gamewidget_ui.cpp
        gamewidget_update.cpp

        # 武器选择界面
        choose.h choose.cpp choose.ui

        resources.qrc
        pause.h pause.cpp pause.ui
        settings.h settings.cpp settings.ui
        upgradewidget.h upgradewidget.cpp
        dynamicbackground.h dynamicbackground.cpp
        coin.h coin.cpp

    )

else()
    if(ANDROID)
        add_library(brotato SHARED
            ${PROJECT_SOURCES}
        )

    else()
        add_executable(brotato
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(brotato PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)


if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.brotato)
endif()
set_target_properties(brotato PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS brotato
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(brotato)
endif()