#include <jni.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "SignalHandler.h"
#include "JNIBridge.h"
#include "CrashAnalyser.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_example_nativecrash_NativeCrashMonitor_nativeInit(JNIEnv *env, jobject nativeCrashMonitor,
                                                           jobject callback) {
    //把callback保存起来，方便回调到java层
    //做一个线程的·监听
    callback = env->NewGlobalRef(callback);
    JavaVM *javaVm;
    env->GetJavaVM(&javaVm);
    jclass nativeCrashMonitorClass = env->GetObjectClass(nativeCrashMonitor);
    nativeCrashMonitorClass = (jclass)env->NewGlobalRef(nativeCrashMonitorClass);
    JNIBridge *jniBridge = new JNIBridge(javaVm, callback, nativeCrashMonitorClass);
    //创建一个线程去监听是否有异常
    initCondition();
    pthread_t  pthread;
    int ret = pthread_create(&pthread, nullptr, threadCrashMonitor, jniBridge);
    if(ret){
      LOGE("pthread_create error, ret:%d", ret);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_nativecrash_NativeCrashMonitor_nativeSetup(JNIEnv *env, jobject thiz) {
    // 设置监听信号量回调处理
    installSignalHandlers();
    // 设置额外的栈空间，让信号处理在单独的栈中处理
    installAlternateStack();
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_nativecrash_NativeCrashMonitor_nativeCrash(JNIEnv *env, jclass clazz) {
    int *num = NULL;
    *num = 100;
}