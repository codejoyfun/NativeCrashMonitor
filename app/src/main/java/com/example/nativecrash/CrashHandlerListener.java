package com.example.nativecrash;

/**
 * @author ningtukun
 * @date 2021/3/31
 */
public interface CrashHandlerListener {
    void onCrash(String threadName, String javaStackInfo, String nativeStackInfo);
}
