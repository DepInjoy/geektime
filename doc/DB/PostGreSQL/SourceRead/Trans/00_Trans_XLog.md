为了应对系统失效，PostgreSQL将所有修改作为历史数据写入持久化存储中，这份历史数据被称为XLOG记录或WAL数据。当插入、删除、提交等变更动作发生时，PostgreSQL会将 XLOG记录写入内存中的 WAL缓冲区。当事务提交或中止时，它们会被立即写入持久化存储的WAL段文件中(其他场景也可能会有XLOG记录写入)。XLOG记录的日志序列号（Log Sequence Number, LSN）标识了该记录在事务日志中的位置，记录的LSN被用作XLOG记录的唯一标识符。

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

```C++
/*
 * Begin constructing a WAL record. This must be called before the
 * XLogRegister* functions and XLogInsert().
 */
void XLogBeginInsert(void) {}

void XLogRegisterData(char *data, int len)
    
XLogRecPtr XLogInsert(RmgrId rmid, uint8 info)
```

