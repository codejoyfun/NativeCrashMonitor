//
// Created by kun on 2021/4/6.
//

#ifndef NATIVECRASH_JNIBRIDGE_H
#define NATIVECRASH_JNIBRIDGE_H
#include <jni.h>
#include <dlfcn.h>
#include <string.h>
#include "CrashDefine.h"
#include "CrashAnalyser.h"


class JNIBridge {
private:
JavaVM *javaVm;
jobject callbackObj;
jclass nativeCrashMonitorClass;
public:
JNIBridge(JavaVM * javaVm, jobject callbackObj, jclass nativeCrashMonitorClass);
public:
void throwException2Java(struct native_handler_context_struct *handlerContext);
};



#endif //NATIVECRASH_JNIBRIDGE_H
