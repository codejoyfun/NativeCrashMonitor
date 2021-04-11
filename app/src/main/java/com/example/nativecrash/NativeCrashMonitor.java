package com.example.nativecrash;

import android.os.Looper;
import android.text.TextUtils;
import android.util.Log;

import androidx.annotation.Keep;

import java.lang.reflect.Field;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * @author ningtukun
 * @date 2021/3/31
 */
public class NativeCrashMonitor {
    private final static String TAG = "NativeCrashMonitor";

    private static ThreadGroup systemThreadGroup;

    static {
        System.loadLibrary("native-lib");
        try {
            Class<?> threadGroupClass = Class.forName("java.lang.ThreadGroup");
            Field systemThreadGroupField = threadGroupClass.getDeclaredField("systemThreadGroup");
            systemThreadGroupField.setAccessible(true);
            systemThreadGroup = (ThreadGroup) systemThreadGroupField.get(null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static volatile boolean isInit = false;

    public void init(CrashHandlerListener callback) {
        if (isInit) return;
        isInit = true;
        nativeInit(callback);
        nativeSetup();
    }

    private native void nativeInit(CrashHandlerListener callback);

    private native void nativeSetup();

    public static native final void nativeCrash();

    @Keep
    private static String getStackInfoByThreadName(String threadName){
        Thread thread = getThreadByName(threadName);
        StringBuilder sb = new StringBuilder();
        StackTraceElement[] stackTraceElements = thread.getStackTrace();
        for (StackTraceElement stackTraceElement : stackTraceElements) {
            sb.append(stackTraceElement.toString()).append("\r\n");
        }
        return sb.toString();
    }

    public static Thread getThreadByName(String threadName) {
        if (TextUtils.isEmpty(threadName)) {
            return null;
        }
        Thread theThread = null;
        if (threadName.equals("main")) {
            theThread = Looper.getMainLooper().getThread();
        } else {
            Thread[] threadArray = new Thread[]{};
            try {
                Set<Thread> threadSet = getAllStackTraces().keySet();
                threadArray = threadSet.toArray(new Thread[threadSet.size()]);
            } catch (Exception e) {
                Log.e(TAG, "dump thread Traces", e);
            }

            for (Thread thread : threadArray) {
                if (thread.getName().equals(threadName)) {
                    theThread = thread;
                    Log.e(TAG, "find it." + threadName);
                }
            }
        }
        return theThread;
    }

    /**
     * 获取线程堆栈的map.
     * @return 返回线程堆栈的map
     */
    private static Map<Thread, StackTraceElement[]> getAllStackTraces() {
        if (systemThreadGroup == null) {
            return Thread.getAllStackTraces();
        } else {
            Map<Thread, StackTraceElement[]> map = new HashMap<>();

            // Find out how many live threads we have. Allocate a bit more
            // space than needed, in case new ones are just being created.
            int count = systemThreadGroup.activeCount();
            Thread[] threads = new Thread[count + count / 2];
            Log.d(TAG, "activeCount: " + count);

            // Enumerate the threads and collect the stacktraces.
            count = systemThreadGroup.enumerate(threads);
            for (int i = 0; i < count; i++) {
                try {
                    map.put(threads[i], threads[i].getStackTrace());
                } catch (Throwable e) {
                    Log.e(TAG, "fail threadName: " + threads[i].getName(), e);
                }
            }
            return map;
        }
    }
}
