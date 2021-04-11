//
// Created by ningtukun on 2021/3/31.
//

#ifndef NATIVECRASH_CRASHDEFINE_H
#define NATIVECRASH_CRASHDEFINE_H

#include <android/log.h>
#include <signal.h>

#define TAG "JNI_TAG"
#define PROCESS_NAME_LENGTH 512
#define THREAD_NAME_LENGTH 512
#define BACKTRACE_FRAMES_MAX 32
#define FORMAT_BUFFSIZE 100
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
//异常信号量
const int exceptionSignals[] = {SIGSEGV, SIGABRT, SIGFPE,SIGILL, SIGBUS, SIGTRAP};
const int exceptionSignalsNumber = sizeof(exceptionSignals) / sizeof(exceptionSignals[0]);

static struct sigaction oldHandlers[NSIG];


#endif //NATIVECRASH_CRASHDEFINE_H
