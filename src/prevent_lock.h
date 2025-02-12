
// Copyright (C) 2025 Andrea Albano

#ifndef PREVENT_LOCK
#define PREVENT_LOCK

#define PREVENT_LOCK_ID     1001

#define PREVENT_LOCK_START_STRING L"Prevent lock"
#define PREVENT_LOCK_STOP_STRING L"Stop prevent lock"

int prevent_lock_is_running();
int prevent_lock_start();
int prevent_lock_stop();


#endif



