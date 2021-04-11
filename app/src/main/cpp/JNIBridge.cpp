//
// Created by kun on 2021/4/6.
//

#include "JNIBridge.h"

JNIBridge::JNIBridge(JavaVM *javaVm, jobject callbackObj, jclass nativeCrashMonitorClass) {
    this->javaVm = javaVm;
    this->callbackObj = callbackObj;
    this->nativeCrashMonitorClass = nativeCrashMonitorClass;
}

void JNIBridge::throwException2Java(struct native_handler_context_struct *handlerContext) {
    JNIEnv *env = NULL;
    if (this->javaVm->AttachCurrentThread(&env, NULL) != JNI_OK) {
        LOGE("AttachCurrentThread failed!");
    }

    //根据java的线程名去拿java线程的堆栈
    const char *sig = "(Ljava/lang/String;)Ljava/lang/String;";
    jmethodID getStackInfoByThreadNameMid = env->GetStaticMethodID(this->nativeCrashMonitorClass,
                                                                   "getStackInfoByThreadName", sig);
    jstring jThreadName = env->NewStringUTF(handlerContext->threadName);
    jobject javaStackInfo = env->CallStaticObjectMethod(this->nativeCrashMonitorClass,
                                                        getStackInfoByThreadNameMid, jThreadName);
    const char *javaExceptionStackInfo = env->GetStringUTFChars((jstring) javaStackInfo, JNI_FALSE);

    //获取c++的堆栈信息
    int frame_size = handlerContext->frame_size;
    char nativeExceptionStackInfo[FORMAT_BUFFSIZE * frame_size];
    for (int index = 0; index < frame_size; ++index) {
        uintptr_t pc = handlerContext->frames[index];
        Dl_info info;
        void *const addr = (void *) (pc);
        if (dladdr(addr, &info) != 0 && info.dli_fname != NULL) {
            const uintptr_t near = (uintptr_t) info.dli_saddr;
            const uintptr_t offs = pc - near;
            uintptr_t addr_rel = pc - (uintptr_t) info.dli_fbase;
            const uintptr_t addr_to_use = is_dll(info.dli_fname) ? addr_rel : pc;
            //todo 还没有办法获得是哪一行 需要用addr2line工具根据pc值获取在哪一行
            char buf[FORMAT_BUFFSIZE];
            snprintf(buf, FORMAT_BUFFSIZE, "native crash #%02lx pc 0x%016lx %s (%s+0x%lx)\n", index,
                     addr_to_use,
                     info.dli_fname, info.dli_sname, offs);
            strcat(nativeExceptionStackInfo, buf);
        }
    }

    jclass callbackClass = env->GetObjectClass(this->callbackObj);
    const char *callbackSig = "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V";
    jmethodID onCrashMid = env->GetMethodID(callbackClass, "onCrash", callbackSig);
    env->CallVoidMethod(this->callbackObj, onCrashMid,
                        charToJstring(env, handlerContext->threadName),
                        charToJstring(env, javaExceptionStackInfo),
                        charToJstring(env, nativeExceptionStackInfo));


    //把java异常堆栈和c++异常堆栈一起抛到回调监听函数
    if (this->javaVm->DetachCurrentThread() != JNI_OK) {
        LOGE("DetachCurrentThread failed!");
    }
}
