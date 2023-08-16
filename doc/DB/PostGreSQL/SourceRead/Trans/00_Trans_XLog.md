```C++
// src/backend/access/transam/xlog.c
bool RecoveryInProgress(void) {
    if (!LocalRecoveryInProgress)
        return false;
    else {
        volatile XLogCtlData *xlogctl = XLogCtl;
        LocalRecoveryInProgress = (xlogctl->SharedRecoveryState
                != RECOVERY_STATE_DONE);

        if (!LocalRecoveryInProgress) {
            pg_memory_barrier();
            InitXLOGAccess();
        }
        return LocalRecoveryInProgress;
    }
}
```

```C++
// 开始事务,调用
void InitXLOGAccess(void)
```