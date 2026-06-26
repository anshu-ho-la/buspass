cmake_minimum_required(VERSION 3.16)

project(buspass VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick)

set(PROJECT_SOURCES
        main.cpp
        login.cpp
        login.h
        login.ui
        signup.ui
        signup.h
        signup.cpp
        mainwindow.ui
        mainwindow.h
        mainwindow.cpp
<<<<<<< HEAD
        review.cpp
        review.h
        review.ui
        adminprofile.cpp
        adminprofile.h
        adminprofile.ui
        userprofile.cpp
        userprofile.h
        userprofile.ui
=======
        pageswitch.h
        adminprofile.cpp
        adminprofile.h
        adminprofile.ui
        review.cpp
        review.h
        review.ui
>>>>>>> 13a2202a57f0061074db60fe6317427a1521662b
    )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

qt_add_executable(buspass
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
            signup.h
            signup.cpp
            signup.ui
            session.h
            database.h
<<<<<<< HEAD
            review.h
            adminprofile.h
            userprofile.ui
=======
            add.h add.cpp add.ui
            adminprofile.h
            review.h
            pageswitch.h
>>>>>>> 13a2202a57f0061074db60fe6317427a1521662b

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET buspass APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(buspass SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(buspass
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(buspass PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.buspass)
endif()
set_target_properties(buspass PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS buspass
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(buspass)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

target_link_libraries(buspass
    PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt${QT_VERSION_MAJOR}::Quick
)