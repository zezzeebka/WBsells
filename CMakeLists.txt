cmake_minimum_required(VERSION 3.14)

project(WBsells LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SFML_DIR SFML-2.5.1/lib/cmake/SFML)
set(CURL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/curl-7.88.0-win64-mingw/include)
set(POSTGRESQL_INCLUDE_DIR "/path/to/postgresql/include")
set(POSTGRESQL_LIB_DIR "/path/to/postgresql/lib")


find_package(SFML COMPONENTS graphics audio window system network REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Core)



#QT

#if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#    qt_add_executable(WBsells
#        MANUAL_FINALIZATION
#        ${PROJECT_SOURCES}
#    )
#else()
#    if(ANDROID)
#        add_library(WBsells SHARED
#            ${PROJECT_SOURCES}
#        )
## Define properties for Android with Qt 5 after find_package() calls as:
##    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    else()
#        add_executable(WBsells
#            ${PROJECT_SOURCES}
#        )
#    endif()
#endif()




include_directories(SFML-2.5.1/include)
include_directories(rapidjson-master/include)

file(GLOB_RECURSE PROJECT_SOURCE src/*.cpp src/*.h data/*.txt data/*.json)

add_executable(WBsells ${PROJECT_SOURCE})
target_link_libraries(WBsells PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(WBsells PRIVATE sfml-graphics sfml-system sfml-window sfml-audio sfml-network)
target_link_libraries(WBsells PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(WBsells PRIVATE Qt${QT_VERSION_MAJOR}::Core)
#target_include_directories(WBsells PRIVATE ${POSTGRESQL_INCLUDE_DIR})
#target_link_directories(WBsells PRIVATE ${POSTGRESQL_LIB_DIR})
#target_link_libraries(WBsells PRIVATE pq)
#target_compile_definitions(WBsells PRIVATE DB_NAME="mydb" DB_USER="myuser" DB_PASSWORD="mypassword")


add_custom_command(TARGET WBsells PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/SFML-2.5.1/bin $<TARGET_FILE_DIR:WBsells>)

add_custom_command(TARGET WBsells PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:WBsells>/data)

TARGET_LINK_DIRECTORIES(WBsells PUBLIC ${CMAKE_SOURCE_DIR}/curl-7.88.0-win64-mingw/lib)
TARGET_LINK_LIBRARIES(WBsells PUBLIC libbrotlicommon.a libbrotlidec.a libcrypto.a libcurl.a libcurl.dll.a libgsasl.a libnghttp2.a libnghttp3.a libngtcp2.a libngtcp2_crypto_openssl.a libssh2.a libssl.a libz.a libzstd.a)
target_include_directories(${PROJECT_NAME} PUBLIC ${CURL_INCLUDE_DIR})
target_link_libraries(WBsells PUBLIC ${CMAKE_SOURCE_DIR}/curl-7.88.0-win64-mingw/lib)

