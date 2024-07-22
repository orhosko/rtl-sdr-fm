cmake_minimum_required(VERSION 3.20)

project(widgets-test VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(widgets-test
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        fm_demod.h fm_demod.cpp
        audiosink.h audiosink.cpp
        rawdataprovider.h rawdataprovider.cpp
        visualizer.h visualizer.cpp
        worker.h worker.cpp
        notes.md
        filewriter.h filewriter.cpp
        fullspectrum.h fullspectrum.cpp
        circularbytearray.h circularbytearray.cpp
        rtlsdr.h rtlsdr.cpp
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET widgets-test APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(widgets-test SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(widgets-test
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(widgets-test PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.widgets-test)
endif()
set_target_properties(widgets-test PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS widgets-test
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(widgets-test)
endif()

find_package(Qt6 REQUIRED COMPONENTS Charts)
target_link_libraries(widgets-test PRIVATE Qt6::Charts)

find_package(Qt6 REQUIRED COMPONENTS Multimedia)
target_link_libraries(widgets-test PRIVATE Qt6::Multimedia)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(widgets-test PRIVATE Qt6::Widgets)

target_include_directories(widgets-test PRIVATE ./Filters)


# find_library(RTLSDRLIB ../rtlsdr/x64/rtlsdr_static.lib)
# find_library(RTLSDRLIB ../lib3rdpartybin/Win64_MinGW/librtlsdr/Debug/librtlsdr.a)

# find_library(RTLSDRLIB ./lib/librtlsdr.dll) # doesnt work

# if(RTLSDRLIB)
#     message("Founded")
# else()
#     message("Not Founded")
# endif()

# target_link_libraries(widgets-test PUBLIC ${RTLSDRLIB})

if(MSVC)
    # target_link_libraries(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/lib/librtlsdr.dll)
    # target_link_libraries(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.dll)
    # target_link_libraries(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/lib/libwinpthread-1.dll)
else()
    target_link_libraries(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/lib/librtlsdr-mingw.dll)

    # target_sources(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/rtlsdr.cpp)
    # target_link_libraries(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/rtlsdr.h)
endif()


target_include_directories(widgets-test PRIVATE include)

set(KISS_FFT "${CMAKE_SOURCE_DIR}/kissfft")

if(MSVC)
    set(Boost_DEBUG ON)
    find_package(Boost REQUIRED)
    add_subdirectory(${KISS_FFT})

    target_link_libraries(widgets-test PRIVATE kissfft Boost::boost)
else()
    add_subdirectory(${KISS_FFT})

    target_link_libraries(widgets-test PRIVATE kissfft)
    target_include_directories(widgets-test PUBLIC ${CMAKE_SOURCE_DIR}/Boost-mingw/boost_1_85_0/)
endif()

set(UHD_DIR "C:/Program Files/UHD")

target_include_directories(
    widgets-test
    PUBLIC
    "${UHD_DIR}/include"
)
target_link_libraries(
    widgets-test
    PUBLIC
    "${UHD_DIR}/lib/uhd.lib"
)

# --------------------------------------------
# --------------------------------------------
# --------------------------------------------
# Set the Boost root directory
# set(BOOST_ROOT "C:/Boost/boost_1_85_0/boost")

# set(Boost_DEBUG ON CACHE BOOL "Enable Boost debug output")

# # Find the Boost package
# find_package(Boost 1.85.0 REQUIRED) # List required components

# target_link_libraries(
#     widgets-test
#     PRIVATE Boost::filesystem)

# target_include_libraries(widgets-test PUBLIC C:/Boost/boost_1_85_0/boost/circular_buffer.hpp)
# target_include_directories(widgets-test PUBLIC C:/Boost/boost_1_85_0/)

# set(BOOST_ROOT "./boost_1_82_0/")
# set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
# set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")

# find_package(
#     Boost REQUIRED
#     COMPONENTS date_time)
# target_link_libraries(
#     widgets-test
#     PRIVATE Boost::date_time)

# add_subdirectory(./boost_1_82_0/)
# target_link_libraries(widgets-test PRIVATE Boost::circular_buffer)


# set(BOOST "${CMAKE_SOURCE_DIR}/../boost_1_82_0/boost")
# target_include_directories(widgets-test PRIVATE BOOST)

# add_subdirectory("./rtaudio-6.0.1")
# target_link_libraries(widgets-test PRIVATE rtaudio)

# file(GLOB KISS_FFT_SOURCES "${KISS_FFT_SRC_DIR}/kiss_fft*.c")

# # Create the executable
# target_sources(widgets-test PRIVATE ${KISS_FFT_SOURCES})

# # Include Kiss FFT headers
# include_directories(${KISS_FFT_SRC_DIR})

# set(LIBRARY_PATH "${CMAKE_SOURCE_DIR}/fftw")

# # Specify the path to your DLL and DEF files
# set(FFTW_DLL "${LIBRARY_PATH}/libfftw3-3.dll")
# set(FFTW_H "${LIBRARY_PATH}/fftw3.h")
# set(FFTW_DEF "${LIBRARY_PATH}/libfftw3-3.def")

# # Check if the DLL file exists
# if(NOT EXISTS "${FFTW_DLL}")
#     message(FATAL_ERROR "DLL not found: ${FFTW_DLL}")
# endif()

# # Check if the DEF file exists
# if(NOT EXISTS "${FFTW_DEF}")
#     message(FATAL_ERROR "DEF file not found: ${FFTW_DEF}")
# endif()

# message("ok")

# add_library(test SHARED ${FFTW_DEF})
# set_target_properties(test PROPERTIES
#     IMPORTED_LOCATION "${FFTW_DLL}"
#     IMPORTED_NO_SONAME TRUE
# )
# target_link_libraries(widgets-test PRIVATE test)

# # Create an imported library to link against the DLL
# add_library(MyLibrary SHARED IMPORTED GLOBAL)
# set_target_properties(MyLibrary PROPERTIES
#     IMPORTED_LOCATION "${FFTW_DLL}"
#     IMPORTED_NO_SONAME TRUE
# )

# # Use LINK_FLAGS to specify the DEF file for the executable
# target_link_options(widgets-test PRIVATE "/DEF:${CMAKE_SOURCE_DIR}/fftw/libfftw3f-3.def")

# # Link the imported library with your executable
# target_link_libraries(widgets-test PRIVATE MyLibrary)
