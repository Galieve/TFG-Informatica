#pragma once
#ifndef DEFINE_H
#define DEFINE_H

#include <vector>
#include <functional>
#include <utility>
#include <memory>
#ifndef NODE_INFO_H
	#include "function/node_info.h"
 #endif

typedef unsigned long long ullong;
typedef std::vector<std::vector<node_info>> vvnode_info;

typedef std::pair<std::shared_ptr<vvnode_info>, std::pair<int, int>> data_task;
typedef std::function<void()> task;

//#define DEBUG_MODE
#define WHERE_AM_I_MODE
#define PATH_FRAGMENT_SIZE 10000
#define SCALE_FACTOR 100000
#define TASK_SIZE 10000
//#define SIMPLE_LOG
//#define FULL_LOG
//#define PRETTY_PRINTING
#define USEFUL_PRINTING
#define LOG_FILENAME "../files/log_file.log"
#define INPUT_SIZE 5
#define FULL_INPUT_SIZE 2*INPUT_SIZE


#endif