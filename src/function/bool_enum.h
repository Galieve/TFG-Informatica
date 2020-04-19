#pragma once
#ifndef BOOL_ENUM_H
#define BOOL_ENUM_H
#include <cassert>

enum bool_enum {undef_, true_, false_};

inline static bool cast(const bool_enum &b){
    switch (b){
    case true_: return true;
    case false_: return false;
    default: assert(false);
    }
}

#endif