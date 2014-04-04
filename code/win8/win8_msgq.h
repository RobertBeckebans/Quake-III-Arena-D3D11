#pragma once

#include <stdint.h>

namespace Q3Win8
{

enum MSG_TYPE
{
    MSG_QUIT,
    MSG_VIDEO_CHANGE
};

struct MSG
{
    MSG_TYPE Message;
    int32_t Param0;
    int32_t Param1;
};

void PostMessage( const MSG* msg );
bool PopMessage( MSG* msg );

}
