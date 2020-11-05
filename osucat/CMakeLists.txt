# CMakeList.txt: osucat-1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

add_definitions(/bigobj)

# 将源代码添加到此项目的可执行文件。
add_executable (osucat
"include/easywsclient/easywsclient.hpp"
"include/easywsclient/easywsclient.cpp"
"include/json/json.hpp"
"include/oppai-cpp/oppai/mods.hpp"
"include/oppai-cpp/oppai/oppai.c"
"include/oppai-cpp/oppai.hpp"
"main.cpp"
"main.h"
"sdk/cqhttp_api.hpp"
"src/osucat.hpp"
"src/main.hpp"
"sdk/utils.hpp"
"sdk/client.hpp"
"src/include/osu_api.hpp"
"src/exception.hpp"
"src/include/winhttp.hpp" "src/include/mysql.hpp" "src/message.h")

# TODO: 如有需要，请添加测试并安装目标。

include_directories(include)
include_directories(extern/Mysql/include)
include_directories(extern/Magick++/include)


target_link_libraries(osucat
        "${CMAKE_CURRENT_SOURCE_DIR}\\extern\\Mysql\\lib\\libmysql.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}\\extern\\Mysql\\lib\\mysqlclient.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}\\extern\\Magick++\\lib\\CORE_RL_Magick++_.lib"
)