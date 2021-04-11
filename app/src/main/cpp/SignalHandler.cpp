//
// Created by ningtukun on 2021/3/31.
//

#include <algorithm>
#include <unistd.h>
#include "SignalHandler.h"


void installAlternateStack() {
    stack_t newStack;
    stack_t oldStack;
    memset(&newStack, 0, sizeof(newStack));
    memset(&oldStack, 0, sizeof(oldStack));
    static const unsigned sigaltstackSize = std::max(16384, SIGSTKSZ);
    //要先把原来的拿出来，可能会有一些其他框架已经设置好了，避免和其他框架冲突，bugly，qapm
    if (sigaltstack(NULL, &oldStack) == -1 || !oldStack.ss_sp ||
        oldStack.ss_size < sigaltstackSize) {
        newStack.ss_sp = calloc(1, sigaltstackSize);
        newStack.ss_size = sigaltstackSize;
        memset(&newStack, 0, sizeof(newStack));
        if (sigaltstack(&newStack, NULL) == -1) {
            //打印警告
            free(newStack.ss_sp);
        }
    }
}

void signalPass(int code, struct siginfo *si, void *sc) {
    LOGI("监听到了native的崩溃");
    //这里要考虑非信号方式防止死锁
    signal(code, SIG_DFL);
    signal(SIGALRM, SIG_DFL);
    (void) alarm(8);
    //解析栈信息，回调给java层，上报到后台或者保存本地文件
    notifyCaughtSignal(code, si, sc);
    //给系统原来的默认处理，否则会进入死循环
    oldHandlers[code].sa_sigaction(code, si, sc);
}

bool installSignalHandlers() {
    //需要保存原来的处理
    for (int i = 0; i < exceptionSignalsNumber; i++) {
        if (sigaction(exceptionSignals[i], NULL, &oldHandlers[exceptionSignals[i]]) == -1) {
            //可以输出一个警告
            return false;
        }
    }
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    //指定信号处理的回调函数
    sa.sa_sigaction = signalPass;
    sa.sa_flags = SA_ONSTACK | SA_SIGINFO;
    //只处理当前关心的信号，其他的不处理
    for (int i = 0; i < exceptionSignalsNumber; i++) {
        sigaddset(&sa.sa_mask, exceptionSignals[i]);
    }
    //1.调用sigaction 来处理信号回调
    for (int i = 0; i < exceptionSignalsNumber; i++) {
        if (sigaction(exceptionSignals[i], &sa, NULL) == -1) {
            //可以输出一个警告

        }
    }

    return false;
}