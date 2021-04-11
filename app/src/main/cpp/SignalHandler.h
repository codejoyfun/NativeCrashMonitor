//
// Created by ningtukun on 2021/3/31.
//

#ifndef NATIVECRASH_SIGNALHANDLER_H
#define NATIVECRASH_SIGNALHANDLER_H

#include "CrashDefine.h"
#include "string.h"
#include "CrashAnalyser.h"

extern bool installSignalHandlers();
extern void installAlternateStack();




#endif //NATIVECRASH_SIGNALHANDLER_H
