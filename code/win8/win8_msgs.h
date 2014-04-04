#pragma once

#include "win8_msgq.h"

namespace Win8
{

enum GAME_MSG
{
    GAME_MSG_QUIT,
    GAME_MSG_VIDEO_CHANGE,
    GAME_MSG_MOUSE_MOVE,
    GAME_MSG_MOUSE_WHEEL,
    GAME_MSG_MOUSE_DOWN,
    GAME_MSG_MOUSE_UP,
    GAME_MSG_KEY_DOWN,
    GAME_MSG_KEY_UP,
    GAME_MSG_KEY_CHAR
};

enum SYS_MSG
{
    SYS_MSG_EXCEPTION,
    SYS_MSG_GAME_READY
};

extern MessageQueue g_gameMsgs;
extern MessageQueue g_sysMsgs;

}
