#pragma once
#ifndef DEFINE_H
#define DEFINE_H

#include <vector>
#include <functional>
#include <utility>
#include <memory>

//#define DEBUG_MODE
#define WHERE_AM_I_MODE
#define PATH_FRAGMENT_SIZE 100000
#define SCALE_FACTOR 10000
#define TASK_SIZE 1000
//#define SIMPLE_LOG
//#define FULL_LOG
//#define PRETTY_PRINTING../files/circuit/
#define FLUSH_FOLDER "../files/production_flush/"
#define MAX_SIZE 500000
#define USEFUL_PRINTING
#define LOG_FILENAME "../files/log/log_file.log"
#define PROD_BITS 5
#define INPUT_SIZE 5
#define FULL_INPUT_SIZE 2*INPUT_SIZE
#if INPUT_SIZE == 1
	#define PRODUCTION_MODE
#endif

#ifndef NODE_INFO_H
	#include "function/node_info.h"
 #endif

typedef unsigned long long ullong;
typedef std::vector<std::vector<node_info>> vvnode_info;

typedef std::pair<std::shared_ptr<vvnode_info>, std::pair<int, int>> data_task;
typedef std::function<void()> task;

#endif
