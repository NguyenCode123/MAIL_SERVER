#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <filesystem>
#include <string>
#include <iterator>
#include <algorithm>
#include<conio.h>
#include<vector>
#include<sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <msclr\marshal_cppstd.h>

using namespace std;
using namespace System;
using namespace System::Data::SqlClient;
using namespace System::Data;
using namespace msclr::interop;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"Crypt32.lib")

const int MAX_BUFFER_SIZE = 1025;