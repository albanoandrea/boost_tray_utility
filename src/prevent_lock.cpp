
// Copyright (C) 2025 Andrea Albano

#include "prevent_lock.h"
#include <windows.h>
#include <shellapi.h>
#include <thread>

static int end_thread = 0;
static std::thread* mouse_move_thread = NULL;
static int thread_running = 0;
static void* prevent_lock_thread()
{
    thread_running = 1;
    POINT cursorPos;
    int toggle = 1;
    SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);
    while (!end_thread)
    {
        for (int i = 0; i < 100; i++)
        {
            Sleep(10);
            if (end_thread)
                break;
        }
        if (end_thread)
            break;
        // Get the current mouse position
        if (!GetCursorPos(&cursorPos)) {
            continue;
        }

        // Move the mouse cursor to the target position
        if (toggle == 1)
            toggle = -1;
        else
            toggle = 1;
        if (!SetCursorPos(cursorPos.x + toggle, cursorPos.y + toggle)) {
            continue;
        }
    }
    thread_running = 0;
    return NULL;
}

int prevent_lock_is_running()
{
    // TODO: add mutex on thread_running
    return thread_running;
}
int prevent_lock_start()
{
    // TODO: handle errors
    end_thread = 0;
    mouse_move_thread = new std::thread(prevent_lock_thread);
    mouse_move_thread->detach();
    return 0;
}
int prevent_lock_stop()
{
    // TODO: handle errors
    if (!mouse_move_thread)
        return 0; // Already stopped
    end_thread = 1;
    while (prevent_lock_is_running())
    {
        Sleep(10);
    }
    delete(mouse_move_thread);
    mouse_move_thread = NULL;
    return 0;
}