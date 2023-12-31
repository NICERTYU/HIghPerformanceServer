cmake_minimum_required(VERSION 3.26)
project(HighPerformanceServer)
set(CMAKE_CXX_STANDARD 17)
FIND_PACKAGE(Boost REQUIRED COMPONENTS date_time filesystem system thread) # COMPONENTS 后面都是库名
find_package(yaml-cpp REQUIRED)

add_executable(HighPerformanceServer main.cpp
        config.h
        config.cpp
        logger.h
        logger.cpp
        util.h
        util.cpp
        singleton.h
        thread.h
        thread.cpp
        superG.h
        macro.h
        fiber.h
        fiber.cpp
        scheduler.h
        scheduler.cpp


)

message(WARNING ${Boost_VERSION_STRING}) # 打印版本号
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) # 包含头文件
    #    add_executable(cliontest main.cpp) # 先添加执行文件
    target_link_libraries(HighPerformanceServer ${Boost_LIBRARIES}) # 再添加库
endif()


include_directories(/usr/local/include/yaml-cpp)




target_link_libraries(HighPerformanceServer /usr/local/lib/libyaml-cpp.a)