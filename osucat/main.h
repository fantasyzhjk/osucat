#pragma once
char OC_ROOT_PATH[512];
bool OC_DEBUG_MODE = false; //开启后会输出原始内容
bool GLOBAL_TEST_MODE = false; //开启后仅作数据处理，不会实际发送
long long int owner_userid;
long long int management_groupid;
char OC_SERVER_HOST[32];
int OC_SERVER_PORT;
char OC_SQL_HOST[32];
int OC_SQL_PORT;
char OC_SQL_DATABASE[16];
char OC_SQL_USER[8];
char OC_SQL_PASSWORD[32];
char OC_OSU_API_KEY[128];
char SC_STEAM_API_KEY[128];

#include <stdio.h>
#include <easywsclient/easywsclient.hpp>
#include <json/json.hpp>
#include <WinSock2.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <memory>
#include <cmath>
#include <random>
#include <time.h>
#include <Windows.h>
#include <optional>
#include <schannel.h>
#include <winhttp.h>
#include <vector>
#include <set>
#include <fstream>
#include <regex>
#include <thread>
#include <codecvt>

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winhttp.lib")

#include "sdk/utils.hpp"
#include "src/exception.hpp"
#include "src/include/winhttp.hpp"
#include "sdk/cqhttp_api.hpp"
#include "src/osucat.hpp"
#include "src/main.hpp"
#include "sdk/client.hpp"



