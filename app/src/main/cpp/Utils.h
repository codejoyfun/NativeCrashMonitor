//
// Created by kun on 2021/4/6.
//

#ifndef NATIVECRASH_UTILS_H
#define NATIVECRASH_UTILS_H
#include <signal.h>
#include <stdlib.h>
#include "CrashAnalyser.h"

extern const char* desc_sig(int sig, int code);

extern const char* getProcessName(pid_t pid);

extern const char* getThreadName(pid_t tid);

extern bool is_dll(const char*  dll_name);
extern jstring charToJstring(JNIEnv *env, const char *pat);

#endif //NATIVECRASH_UTILS_H
