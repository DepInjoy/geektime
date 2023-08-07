每当事务开始(当执行BEGIN命令后的第一条命令)时，事务管理器就会为其分配一个称为事务标识（transaction id, txid）的唯一标识符。PostgreSQL的txid 是一个32位无符号整数，取值空间大小约为42亿。PostgreSQL保留以下三个特殊txid：
- 0表示无效的txid，表示尚未删除或更新此元组。
- 1表示初始启动的txid，仅用于数据库集群的初始化过程。
- 2表示冻结的txid，用于解决事务回卷问题，非活跃事务，结果对于其他事务可见。

txid 可以相互比较大小，大于txid属于"未来"，对当前事务不可见，小于txid属于过去，对当前事务可见。txid 在逻辑上是无限的，而实际系统中的txid 空间不足（4B整型的取值空间大小约42亿），因此PostgreSQL将txid空间视为一个环。对于某个特定的txid，其前约21亿个txid属于过去，其后约21亿个txid属于未来

```c
// 事务id用uint32表示
typedef uint32 TransactionId;

// PG保留了三个特殊的事务ID
// 0表示无效txid, 1表示初始启动txid, 2表示冻结txid
#define InvalidTransactionId		((TransactionId) 0)
#define BootstrapTransactionId		((TransactionId) 1)
#define FrozenTransactionId			((TransactionId) 2)


#define FirstNormalTransactionId	((TransactionId) 3)
#define MaxTransactionId			((TransactionId) 0xFFFFFFFF)

typedef uint32 LocalTransactionId;

// 子事务ID,用于设置检查点
typedef uint32 SubTransactionId;
#define InvalidSubTransactionId		((SubTransactionId) 0)
#define TopSubTransactionId			((SubTransactionId) 1)

typedef uint32 CommandId;
#define FirstCommandId	((CommandId) 0)
#define InvalidCommandId	(~(CommandId)0)
```

```C++
// src/include/access/transam.h
// 64位, 由epoch(前32位)和TransactionId(后32位)组成
typedef struct FullTransactionId {
	uint64		value;
} FullTransactionId;

#define EpochFromFullTransactionId(x)	((uint32) ((x).value >> 32))
#define XidFromFullTransactionId(x)		((uint32) (x).value)
```




```C++
// 回到父事务
static void PopTransaction(void) {
  TransactionState s = CurrentTransactionState;
  CurrentTransactionState = s->parent;
  CurTransactionContext = s->parent->curTransactionContext;
  CurTransactionResourceOwner = s->parent->curTransactionOwner;
  CurrentResourceOwner = s->parent->curTransactionOwner;
  			......
}
```



```C++
// 执行savepoint命令
void DefineSavepoint(const char *name)
```



```C
// src/backend/access/transam/xact.c
// GetCurrentTransactionId
//	-> AssignTransactionId
//		-> GetNewTransactionId
TransactionId GetCurrentTransactionId(void){
	TransactionState s = CurrentTransactionState;
    // txid无效,生成新的txid
	if (!FullTransactionIdIsValid(s->fullTransactionId))
		AssignTransactionId(s);
	return XidFromFullTransactionId(s->fullTransactionId);
}

static void AssignTransactionId(TransactionState s) {
    
}


// src/backend/access/transam/varsup.c
// 为新事务或子事务分配FullTransactionId, 需要获取XidGenLock锁
FullTransactionId GetNewTransactionId(bool isSubXact) {
    
}
```

# 快照

```C
// src/include/utils/snapshot.h
typedef struct SnapshotData *Snapshot;
typedef struct SnapshotData {
	SnapshotType snapshot_type; /* type of snapshot */

	// 最小活跃事务的txid, 所有txid<xmin,对于当前事务都可见
	TransactionId xmin;			/* all XID < xmin are visible to me */
	// 第一个尚未分配的txid,所有txid>xmax的事务, 对当前事务都不可见
	TransactionId xmax;			/* all XID >= xmax are invisible to me */
	// 获取快照时,活跃事务的txid列表
	// for all ids in xip[] satisfy xmin <= xip[i] < xmax
	TransactionId *xip;
	uint32		xcnt;			/* # of xact ids in xip[] */

	/*
	 * For non-historic MVCC snapshots, this contains subxact IDs that are in
	 * progress (and other transactions that are in progress if taken during
	 * recovery). For historic snapshot it contains *all* xids assigned to the
	 * replayed transaction, including the toplevel xid.
	 *
	 * note: all ids in subxip[] are >= xmin, but we don't bother filtering
	 * out any that are >= xmax
	 */
	TransactionId *subxip;
	int32		subxcnt;		/* # of xact ids in subxip[] */
	bool		suboverflowed;	/* has the subxip array overflowed? */

	bool		takenDuringRecovery;	/* recovery-shaped snapshot? */
	bool		copied;			/* false if it's a static snapshot */
	// 
	CommandId	curcid;			/* in my xact, CID < curcid are visible */

	/*
	 * An extra return value for HeapTupleSatisfiesDirty, not used in MVCC
	 * snapshots.
	 */
	uint32		speculativeToken;

	/*
	 * For SNAPSHOT_NON_VACUUMABLE (and hopefully more in the future) this is
	 * used to determine whether row could be vacuumed.
	 */
	struct GlobalVisState *vistest;

	/*
	 * Book-keeping information, used by the snapshot manager
	 */
	uint32		active_count;	/* refcount on ActiveSnapshot stack */
	uint32		regd_count;		/* refcount on RegisteredSnapshots */
	pairingheap_node ph_node;	/* link in the RegisteredSnapshots heap */

	TimestampTz whenTaken;		/* timestamp when snapshot was taken */
	XLogRecPtr	lsn;			/* position in the WAL stream when taken */

	/*
	 * The transaction completion count at the time GetSnapshotData() built
	 * this snapshot. Allows to avoid re-computing static snapshots when no
	 * transactions completed since the last GetSnapshotData().
	 */
	uint64		snapXactCompletionCount;
} SnapshotData;
```



```C++
// src/backend/storage/ipc/procarray.c
Snapshot GetSnapshotData(Snapshot snapshot) {
    	......
    return snapshot;
}
```

```c
// src/backend/storage/ipc/procarray.c
// 其后存在一个pgprocnos变量定义记录在allProcs[]中的index
static ProcArrayStruct *procArray;

// 指向ProcGlobal->allProcs相同的地址
// 参见CreateSharedProcArray
static PGPROC *allProcs;

// src/include/storage/proc.h

// 整个Database cluster有一个ProcGlobal数据结构
// 向
extern PGDLLIMPORT PROC_HDR *ProcGlobal;
```

```C
// src/backend/storage/ipc/procarray.c
typedef struct ProcArrayStruct {
    // 记录其在allProcs[]中的index
    int			pgprocnos[FLEXIBLE_ARRAY_MEMBER];
}
```



```C
// src/include/storage/proc.h
// 每个backend在shared memory都存有一个PGPROC数据结构
struct PGPROC {
};

// 整个Database cluster有一个ProcGlobal数据结构
typedef struct PROC_HDR {
    
}
extern PGDLLIMPORT PROC_HDR *ProcGlobal;
```



```C++
// src/include/storage/proc.h
struct PGPROC {
	/* proc->links MUST BE FIRST IN STRUCT (see ProcSleep,ProcWakeup,etc) */
	SHM_QUEUE	links;			/* list link if process is in a list */
	PGPROC	  **procgloballist; /* procglobal list that owns this PGPROC */

	PGSemaphore sem;			/* ONE semaphore to sleep on */
	ProcWaitStatus waitStatus;

	Latch		procLatch;		/* generic latch for process */


	TransactionId xid;			/* id of top-level transaction currently being
								 * executed by this proc, if running and XID
								 * is assigned; else InvalidTransactionId.
								 * mirrored in ProcGlobal->xids[pgxactoff] */

	TransactionId xmin;			/* minimal running XID as it was when we were
								 * starting our xact, excluding LAZY VACUUM:
								 * vacuum must not remove tuples deleted by
								 * xid >= xmin ! */

	LocalTransactionId lxid;	/* local id of top-level transaction currently
								 * being executed by this proc, if running;
								 * else InvalidLocalTransactionId */
	int			pid;			/* Backend's process ID; 0 if prepared xact */

	int			pgxactoff;		/* offset into various ProcGlobal->arrays with
								 * data mirrored from this PGPROC */
	int			pgprocno;

	/* These fields are zero while a backend is still starting up: */
	BackendId	backendId;		/* This backend's backend ID (if assigned) */
	Oid			databaseId;		/* OID of database this backend is using */
	Oid			roleId;			/* OID of role using this backend */

	Oid			tempNamespaceId;	/* OID of temp schema this backend is
									 * using */

	bool		isBackgroundWorker; /* true if background worker. */

	/*
	 * While in hot standby mode, shows that a conflict signal has been sent
	 * for the current transaction. Set/cleared while holding ProcArrayLock,
	 * though not required. Accessed without lock, if needed.
	 */
	bool		recoveryConflictPending;

	/* Info about LWLock the process is currently waiting for, if any. */
	bool		lwWaiting;		/* true if waiting for an LW lock */
	uint8		lwWaitMode;		/* lwlock mode being waited for */
	proclist_node lwWaitLink;	/* position in LW lock wait list */

	/* Support for condition variables. */
	proclist_node cvWaitLink;	/* position in CV wait list */

	/* Info about lock the process is currently waiting for, if any. */
	/* waitLock and waitProcLock are NULL if not currently waiting. */
	LOCK	   *waitLock;		/* Lock object we're sleeping on ... */
	PROCLOCK   *waitProcLock;	/* Per-holder info for awaited lock */
	LOCKMODE	waitLockMode;	/* type of lock we're waiting for */
	LOCKMASK	heldLocks;		/* bitmask for lock types already held on this
								 * lock object by this backend */
	pg_atomic_uint64 waitStart; /* time at which wait for lock acquisition
								 * started */

	int			delayChkpt;		/* for DELAY_CHKPT_* flags */

	uint8		statusFlags;	/* this backend's status flags, see PROC_*
								 * above. mirrored in
								 * ProcGlobal->statusFlags[pgxactoff] */

	/*
	 * Info to allow us to wait for synchronous replication, if needed.
	 * waitLSN is InvalidXLogRecPtr if not waiting; set only by user backend.
	 * syncRepState must not be touched except by owning process or WALSender.
	 * syncRepLinks used only while holding SyncRepLock.
	 */
	XLogRecPtr	waitLSN;		/* waiting for this LSN or higher */
	int			syncRepState;	/* wait state for sync rep */
	SHM_QUEUE	syncRepLinks;	/* list link if process is in syncrep queue */

	/*
	 * All PROCLOCK objects for locks held or awaited by this backend are
	 * linked into one of these lists, according to the partition number of
	 * their lock.
	 */
	SHM_QUEUE	myProcLocks[NUM_LOCK_PARTITIONS];

	XidCacheStatus subxidStatus;	/* mirrored with
									 * ProcGlobal->subxidStates[i] */
	struct XidCache subxids;	/* cache for subtransaction XIDs */

	/* Support for group XID clearing. */
	/* true, if member of ProcArray group waiting for XID clear */
	bool		procArrayGroupMember;
	/* next ProcArray group member waiting for XID clear */
	pg_atomic_uint32 procArrayGroupNext;

	/*
	 * latest transaction id among the transaction's main XID and
	 * subtransactions
	 */
	TransactionId procArrayGroupMemberXid;

	uint32		wait_event_info;	/* proc's wait information */

	/* Support for group transaction status update. */
	bool		clogGroupMember;	/* true, if member of clog group */
	pg_atomic_uint32 clogGroupNext; /* next clog group member */
	TransactionId clogGroupMemberXid;	/* transaction id of clog group member */
	XidStatus	clogGroupMemberXidStatus;	/* transaction status of clog
											 * group member */
	int			clogGroupMemberPage;	/* clog page corresponding to
										 * transaction id of clog group member */
	XLogRecPtr	clogGroupMemberLsn; /* WAL location of commit record for clog
									 * group member */

	/* Lock manager data, recording fast-path locks taken by this backend. */
	LWLock		fpInfoLock;		/* protects per-backend fast-path state */
	uint64		fpLockBits;		/* lock modes held for each fast-path slot */
	Oid			fpRelId[FP_LOCK_SLOTS_PER_BACKEND]; /* slots for rel oids */
	bool		fpVXIDLock;		/* are we holding a fast-path VXID lock? */
	LocalTransactionId fpLocalTransactionId;	/* lxid for fast-path VXID
												 * lock */

	/*
	 * Support for lock groups.  Use LockHashPartitionLockByProc on the group
	 * leader to get the LWLock protecting these fields.
	 */
	PGPROC	   *lockGroupLeader;	/* lock group leader, if I'm a member */
	dlist_head	lockGroupMembers;	/* list of members, if I'm a leader */
	dlist_node	lockGroupLink;	/* my member link, if I'm a member */
};

// src/backend/storage/ipc/procarray.c
typedef struct ProcArrayStruct {
	int			numProcs;		/* number of valid procs entries */
	int			maxProcs;		/* allocated size of procs array */

	/*
	 * Known assigned XIDs handling
	 */
	int			maxKnownAssignedXids;	/* allocated size of array */
	int			numKnownAssignedXids;	/* current # of valid entries */
	int			tailKnownAssignedXids;	/* index of oldest valid element */
	int			headKnownAssignedXids;	/* index of newest element, + 1 */
	slock_t		known_assigned_xids_lck;	/* protects head/tail pointers */

	/*
	 * Highest subxid that has been removed from KnownAssignedXids array to
	 * prevent overflow; or InvalidTransactionId if none.  We track this for
	 * similar reasons to tracking overflowing cached subxids in PGPROC
	 * entries.  Must hold exclusive ProcArrayLock to change this, and shared
	 * lock to read it.
	 */
	TransactionId lastOverflowedXid;

	/* oldest xmin of any replication slot */
	TransactionId replication_slot_xmin;
	/* oldest catalog xmin of any replication slot */
	TransactionId replication_slot_catalog_xmin;

	/* indexes into allProcs[], has PROCARRAY_MAXPROCS entries */
    // 保存的
	int			pgprocnos[FLEXIBLE_ARRAY_MEMBER];
} ProcArrayStruct;
```

