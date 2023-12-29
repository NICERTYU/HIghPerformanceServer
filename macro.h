//
// Created by GW on 2023/12/29.
//

#ifndef HIGHPERFORMANCESERVER_MACRO_H
#define HIGHPERFORMANCESERVER_MACRO_H
#include<string.h>
#include "util.h"
#include "assert.h"

#define SUPERG_ASSERT(x) \
     if(!(x))            \
     { SUPERG_LOG_ERROR(SUPERG_LOG_ROOT()) <<"ASSERTION:" #x<< \
     "\nbacktrace\n"  \
     <<superG::BacktraceToString(100,"    ",2);\
          assert(x)    ;           \
     }

#define SUPERG_ASSERT1(x,w) \
     if(!(x))            \
     { SUPERG_LOG_ERROR(SUPERG_LOG_ROOT()) <<"ASSERTION:" #x<<"\n"<<w<< \
     "\nbacktrace\n"  \
     <<superG::BacktraceToString(100,"    ",2);\
          assert(x)  ;             \
     }

#endif //HIGHPERFORMANCESERVER_MACRO_H
