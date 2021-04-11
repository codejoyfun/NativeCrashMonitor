//
// Created by kun on 2021/4/6.
//

#include "CrashAnalyser.h"

//锁的条件变量
//互斥锁
pthread_cond_t signalCond;
pthread_mutex_t signalLock;
pthread_cond_t exceptionCond;
pthread_mutex_t exceptionLock;

native_handler_context *handlerContext;

_Unwind_Reason_Code unwind_callback(struct _Unwind_Context *context, void *arg) {
    native_handler_context *const s = static_cast<native_handler_context *const>(arg);
    const uintptr_t pc = _Unwind_GetIP(context);
    if (pc != 0x0) {
        //把pc值保存到native_handler_context
        s->frames[s->frame_size++] = pc;
        LOGE("pc = %p", pc);
    }

    if(s->frame_size == BACKTRACE_FRAMES_MAX){
        return _URC_END_OF_STACK;
    }else{
        return _URC_NO_REASON;
    }
}

/**
 * 初始化锁的条件变量和互斥锁
 */
void initCondition() {
    handlerContext = (native_handler_context *) (malloc(
            sizeof(native_handler_context_struct)));
    pthread_mutex_init(&signalLock, NULL);
    pthread_mutex_init(&exceptionLock, NULL);

    pthread_cond_init(&signalCond, NULL);
    pthread_cond_init(&exceptionCond, NULL);
}

void *threadCrashMonitor(void *argv) {
    //等待有crash捕获的监听
    JNIBridge *jniBridge = static_cast<JNIBridge *>(argv);
    while (true) {
        //等待被唤醒
        waitForSignal();
        //解析异常信息，堆栈
        analysisNativeException();
        //抛给java层
        jniBridge->throwException2Java(handlerContext);
    }
    int status = 1;
    return &status;
}

void waitForSignal() {
    pthread_mutex_lock(&signalLock);
    LOGD("waitForsignal start");
    pthread_cond_wait(&signalCond, &signalLock);
    LOGD("waitForsignal finish");
    pthread_mutex_unlock(&signalLock);
}

/**
 * 唤醒等待
 */
void notifyCaughtSignal(int code, struct siginfo *si, void *sc) {
    //创建一个结构体来保存参数
    copyInfo2Context(code, si, sc);
    pthread_mutex_lock(&signalLock);
    LOGD("notifyCaughtSignal");
    pthread_cond_signal(&signalCond);

    pthread_mutex_unlock(&signalLock);
}

void copyInfo2Context(int code, siginfo *si, void *sc) {
    handlerContext->code = code;
    handlerContext->si = si;
    handlerContext->sc = sc;
    //解析进程和线程 processName threadName
    handlerContext->pid = getpid();
    handlerContext->tid = gettid();
    handlerContext->processName = getProcessName(handlerContext->pid);
    if (handlerContext->pid == handlerContext->tid) {//主线程闪退，线程id和进程id相同
        handlerContext->threadName = "main";
    } else {
        handlerContext->threadName = getThreadName(handlerContext->tid);
    }
    handlerContext->frame_size = 0;
    _Unwind_Backtrace(unwind_callback, handlerContext);
}

void analysisNativeException() {
    //解析
    //crash原因，空指针
    const char *posixDesc = desc_sig(handlerContext->si->si_signo, handlerContext->si->si_code);
    LOGD("posixDesc -> %s", posixDesc);
    LOGD("signal -> %d", handlerContext->si->si_signo);
    LOGD("address -> %p", handlerContext->si->si_addr);
    LOGD("processName -> %s", handlerContext->processName);
    LOGD("threadName -> %s", handlerContext->threadName);
    LOGD("pid -> %d", handlerContext->pid);
    LOGD("tid -> %d", handlerContext->tid);
}

