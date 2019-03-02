set(TARGET teamcity_boost)
file(GLOB SOURCES *.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../include/${TARGET}/*.h)
 
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include 
    ${Boost_INCLUDE_DIRS}) 

add_library(${TARGET} ${SOURCES})
