"# NativeCrashMonitor" 
```
NativeCrashMonitor nativeCrashMonitor = new NativeCrashMonitor();
//threadName 线程名 javaStackInfo java堆栈 nativeStackInfo native堆栈
nativeCrashMonitor.init((threadName, javaStackInfo, nativeStackInfo) -> {
    Log.i("tag", "\n" +"threadName: " + threadName + "\n" + "javaStackInfo:\n" + javaStackInfo + "\n" +"nativeStackInfo:\n" + nativeStackInfo);
});
```
结合addr2line工具就能准确定位native崩溃代码
