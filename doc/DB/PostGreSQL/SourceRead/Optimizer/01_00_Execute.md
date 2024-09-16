```c
typedef enum PortalStrategy {
  PORTAL_ONE_SELECT,
  PORTAL_ONE_RETURNING,
  PORTAL_ONE_MOD_WITH,
  PORTAL_UTIL_SELECT,
  PORTAL_MULTI_QUERY
} PortalStrategy;
```

```c
// src/include/tcop/pquery.h
extern void PortalStart(Portal portal, ParamListInfo params,
    int eflags, Snapshot snapshot);

extern void PortalSetResultFormat(Portal portal, int nFormats,
    int16 *formats);

extern bool PortalRun(Portal portal, long count, bool isTopLevel,
    bool run_once, DestReceiver *dest, DestReceiver *altdest,
    QueryCompletion *qc);

extern uint64 PortalRunFetch(Portal portal,
    FetchDirection fdirection, long count, DestReceiver *dest);
```

```c
// Execute a "simple Query" protocol message.
static void exec_simple_query(const char *query_string) {
        .....
  // 1. Start Portal
  PortalStart(portal, NULL, 0, InvalidSnapshot);

  // 2. 
  PortalSetResultFormat(portal, 1, &format);

  // 3. 创建DestReceiver，接受TupleTableSlot
  //    Executor通过该接口向外发送结果
  receiver = CreateDestReceiver(dest);
  
  // Run the portal to completion, and then drop it (and the receiver).
  (void) PortalRun(portal, FETCH_ALL, true, true,
    receiver, receiver, &qc);
  receiver->rDestroy(receiver);

  PortalDrop(portal, false);
}
```

# DestReceiver
首先，了解相关数据结构
```c
typedef struct _DestReceiver DestReceiver;
struct _DestReceiver {
  // 接受一行结果，执行器通过该接口从孩子拉取数据
  bool (*receiveSlot)(TupleTableSlot *slot, DestReceiver *self);
  // 每个Executor接受结果前，进行初始化
  void (*rStartup) (DestReceiver *self, int operation,
      TupleDesc typeinfo);
  // 接受完结果后调用，用于清理和关闭
  void (*rShutdown) (DestReceiver *self);
  // 销毁Receiver自身
  void (*rDestroy) (DestReceiver *self);
  // 表示具体的Dest类型
  CommandDest mydest;
};

typedef enum {
  // 丢弃处理结果
  DestNone, 
  DestDebug,          /* results go to debugging output */

  // 用于大多数标准查询，如 SELECT 语句，返回复杂的结果集
  DestRemote,          /* results sent to frontend process */
  DestRemoteExecute,      /* sent to frontend, in Execute command */
  
  // 用于简单命令
  // 如INSERT、UPDATE、DELETE不需返回结果集或只返回简单结果
  DestRemoteSimple,      /* sent to frontend, w/no catalog access */
  DestSPI,          /* results sent to SPI manager */
  DestTuplestore,        /* results sent to Tuplestore */

  // 将结果插入指定的目标表中(SELECT INTO)
  DestIntoRel,
  DestCopyOut,        /* results sent to COPY TO code */
  DestSQLFunction,      /* results sent to SQL-language func mgr */
  DestTransientRel,      /* results sent to transient relation */
  DestTupleQueue        /* results sent to tuple queue */
} CommandDest;
```

创建DR的接口实现
```c
DestReceiver * CreateDestReceiver(CommandDest dest) {
  switch (dest) {
    // printtup，将Tuple数据发送给客户端
    case DestRemote:
    case DestRemoteExecute:
      return printtup_create_DR(dest);

    // 
    case DestRemoteSimple:
      return unconstify(DestReceiver *, &printsimpleDR);

    // 丢弃处理结果,空实现
    case DestNone:
      return unconstify(DestReceiver *, &donothingDR);

    case DestDebug:
      return unconstify(DestReceiver *, &debugtupDR);

    case DestSPI:
      return unconstify(DestReceiver *, &spi_printtupDR);

    case DestTuplestore:
      return CreateTuplestoreDestReceiver();

    case DestIntoRel:
      return CreateIntoRelDestReceiver(NULL);

    case DestCopyOut:
      return CreateCopyDestReceiver();

    case DestSQLFunction:
      return CreateSQLFunctionDestReceiver();

    case DestTransientRel:
      return CreateTransientRelDestReceiver(InvalidOid);

    case DestTupleQueue:
      return CreateTupleQueueDestReceiver(NULL);
  }
}
```

## printtup DR
```c
DestReceiver * printtup_create_DR(CommandDest dest) {
  DR_printtup *self = (DR_printtup *) palloc0(sizeof(DR_printtup));
  self->pub.receiveSlot = printtup;
  self->pub.rStartup = printtup_startup;
  self->pub.rShutdown = printtup_shutdown;
  self->pub.rDestroy = printtup_destroy;
  self->pub.mydest = dest;
  self->sendDescrip = (dest == DestRemote);
          ......

  return (DestReceiver *) self;
}
```

## DestRemoteSimple

相应的接口定义：
```c
static const DestReceiver printsimpleDR = {
  printsimple, printsimple_startup, donothingCleanup, 
  donothingCleanup, DestRemoteSimple
};
```
也就是说通过`printsimple`接口实现接收`TupleTableSlot`。
```c
// For each tuple, send a DataRow message.
bool printsimple(TupleTableSlot *slot, DestReceiver *self) {
                    ......
  for (i = 0; i < tupdesc->natts; ++i) {
    pq_sendxxx();
  }
                    ......
  pq_endmessage(&buf);
}

// src/backend/libpq/pqformat.c
// send the completed message to the frontend
void pq_endmessage(StringInfo buf) {
  /* msgtype was saved in cursor field */
  (void) pq_putmessage(buf->cursor, buf->data, buf->len);
  pfree(buf->data);
  buf->data = NULL;
}
```
其中
```c
#define pq_putmessage(msgtype, s, len) \
    (PqCommMethods->putmessage(msgtype, s, len))
```

```c
// src/backend/libpq/pqcomm.c
static const PQcommMethods PqCommSocketMethods = {
  socket_comm_reset,
  socket_flush,
  socket_flush_if_writable,
  socket_is_send_pending,
  // 实现发送消息
  socket_putmessage,
  socket_putmessage_noblock
};
const PQcommMethods *PqCommMethods = &PqCommSocketMethods;
```