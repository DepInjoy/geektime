
默认的page的大小是 8K。



```c
typedef struct PageHeaderData {
	/* XXX LSN is member of *any* block, not only page-organized ones */
	PageXLogRecPtr pd_lsn;		/* LSN: next byte after last byte of xlog
								 * record for last change to this page */
	uint16		pd_checksum;	/* checksum */
	uint16		pd_flags;		/* flag bits, see below */
	LocationIndex pd_lower;		/* offset to start of free space */
	LocationIndex pd_upper;		/* offset to end of free space */
	LocationIndex pd_special;	/* offset to start of special space */
	uint16		pd_pagesize_version;
	TransactionId pd_prune_xid; /* oldest prunable XID, or zero if none */
	ItemIdData	pd_linp[FLEXIBLE_ARRAY_MEMBER]; /* line pointer array */
} PageHeaderData;

typedef PageHeaderData *PageHeader;
```



```c
/* pointer to an item within a disk page of a known file
 * ip_blkid tells us which block, ip_posid tells us which entry in
 * the linp (ItemIdData) array we want
 */
typedef struct ItemPointerData {
	BlockIdData ip_blkid;
    // 元组在页面偏移量
	OffsetNumber ip_posid;
}
```



```c
static inline Item
PageGetItem(Page page, ItemId itemId) {
	return (Item) (((char *) page) + ItemIdGetOffset(itemId));
}
```



# 元组

## 元组数据结构

一个完整的元组信息对应

- 一个`HeapTupleData`是元组在内存中的拷贝，是磁盘格式元组读入内存后的存在方式，PG通过编程技巧巧妙低将元组的实际数据存放在`HeapTupleHeaderData`。它包含一个`HeapTupleHeaderData`，其中比较中重要的字段：
  - 一个具有2个成员的联合类型的`t_choice`:
    - `t_heap`：用于记录对元组执行插入/删除操作事务ID和command ID，主要用于并发控制是检查元组对事物的可见性
    - `t_datum`：一个新元组在内存中形成时，我们不关心事务可见性，`t_choice`中用`DatumTupleFields`结构来记录元组的长度等信息，把内存的数据写入到表文件的时候，需要在元组中记录事务ID和命令ID，把`t_choice`所占的内存转换成`HeapTupleFields`结构并且填充相应数据后再插入元组。

- 一个`TupleDescData`,元组描述符，记录元组相关的全部属性模式信息。通过元组描述符可以读取磁盘中存储的无格式数据，并根据元组描述符构造出元组的各个属性。



```c
// src/include/access/htup_details.h
struct HeapTupleHeaderData {
	union {
        // 用于并发控制时检查元组对事务的可见性
		HeapTupleFields t_heap;
        // 新元组在内存中形成时记录元组的长度等信息
		DatumTupleFields t_datum;
	} t_choice;
	// 当前元组或更新元组的TID(记录当前元组块号和块内偏移量)
	ItemPointerData t_ctid;

	/* Fields below here must match MinimalTupleData! */
#define FIELDNO_HEAPTUPLEHEADERDATA_INFOMASK2 2
    // 用低11位表示当前元组的属性个数
    // 其他位用于包括HOT技术以及元组可见性的标志位
	uint16		t_infomask2;

#define FIELDNO_HEAPTUPLEHEADERDATA_INFOMASK 3
    // 用于表示元组当前状态，例如元组是否有OID、是否有空属性等
    // 每一位对应不同的状态，共16种状态
	uint16		t_infomask;

#define FIELDNO_HEAPTUPLEHEADERDATA_HOFF 4
    // 元组头大小
	uint8		t_hoff;

	/* ^ - 23 bytes - ^ */
#define FIELDNO_HEAPTUPLEHEADERDATA_BITS 5
	bits8		t_bits[FLEXIBLE_ARRAY_MEMBER];	/* bitmap of NULLs */

	/* MORE DATA FOLLOWS AT END OF STRUCT */
};

// src/include/access/htup.h
typedef struct HeapTupleHeaderData HeapTupleHeaderData;
typedef HeapTupleHeaderData *HeapTupleHeader;

typedef struct MinimalTupleData MinimalTupleData;
typedef MinimalTupleData *MinimalTuple;

// 元组在内存中的拷贝，是磁盘格式元组读入内存后的存在方式
typedef struct HeapTupleData {
    // 当前元组的长度
	uint32		t_len;
    // 记录块号以及元组在块内的偏移量
	ItemPointerData t_self;
    // 记录元组所在表的OID
	Oid			t_tableOid;
#define FIELDNO_HEAPTUPLEDATA_DATA 3
    // 记录元组头信息
	HeapTupleHeader t_data;
} HeapTupleData;
typedef HeapTupleData *HeapTuple;

#define HEAPTUPLESIZE	MAXALIGN(sizeof(HeapTupleData))
```



```c
// src/include/access/htup_details.h
typedef struct HeapTupleFields {
    // 插入当前tuple的事务ID
	TransactionId t_xmin;
    // 删除或锁定当前tuple的事务ID
	TransactionId t_xmax;
	union {
        // 插入或删除的command id
		CommandId	t_cid;
        // 老式VACUUM FULL的事务ID
		TransactionId t_xvac;
	} t_field3;
} HeapTupleFields;

typedef struct DatumTupleFields {
	int32		datum_len_;		/* varlena header (do not touch directly!) */

	int32		datum_typmod;	/* -1, or identifier of a record type */

	Oid			datum_typeid;	/* composite type OID, or RECORDOID */
} DatumTupleFields;
```





```c
typedef struct TupleDescData {
    // 元组的属性个数
	int			natts;
    // 元组的复合类型ID
    // PG自动为每个表建立一个行类型，以表示该表的行结构
	Oid			tdtypeid;		/* composite type ID for tuple type */
    // 元组模式
    // tdtypeid有值时，tdtypmod置为-1
    // tdtypeid为默认值RECORDOID时，tdtypmod可为-1或其在typecache中的位置
	int32		tdtypmod;
    // 元组描述符引用计数
    // 只有引用计数为0,该元组描述符才能被删除
	int			tdrefcount;
    // 元组的约束条件，从pg_constraint系统表读取
    // 每个元素表示一个约束条件，NULL表示无
	TupleConstr *constr;
    // 元组每个属性相关信息，从pg_attribute系统表读取
	FormData_pg_attribute attrs[FLEXIBLE_ARRAY_MEMBER];
} TupleDescData;
typedef struct TupleDescData *TupleDesc;
```

## 元组操作

### 插入元组

1. 插入元组前，首先需要根据元组内数据和描述符等信息初始化`HeapTuple`，`heap_form_tuple`实现这一功能，其调用`heap_compute_data_size`计算形成元组锁需要的内存大小，之后为内存分配足够空间。完成必要元组头部设置后，调用`heap_fill_tuple`向元组填充实际数据。

   ```c
   /**
    * values:	将要插入的元组的各属性值数组
    * isnull:  标识哪些属性为空值的数组
    *
    * 主要两个接口调用:
    *	1. heap_compute_data_size计算形成元组锁需要的内存大小
    *	2. heap_fill_tuple向元组填充实际数据
   */
   HeapTuple heap_form_tuple(TupleDesc tupleDescriptor, Datum *values,bool *isnull)
   ```

2. 准备向表中插入元组，`heap_insert`实现这一功能。

   ```c
   void heap_insert(Relation relation, HeapTuple tup, CommandId cid,
   			int options, BulkInsertState bistate) {
       // 1. 获取当前事务ID
   	TransactionId xid = GetCurrentTransactionId();
   	HeapTuple	heaptup;
   	Buffer		buffer;
   	Buffer		vmbuffer = InvalidBuffer;
   	bool		all_visible_cleared = false;
   
   	// 2. 初始化Tuple, 设置t_xmin, t_xmax, t_cid, tableoid
   	heaptup = heap_prepare_insert(relation, tup, xid, cid, options);
   
   
   	// 3. 找到属于该表且空闲空间(Freespace)大于newtup的文件块
   	//    将其载入缓冲区, 用来插入tuple
   	buffer = RelationGetBufferForTuple(relation, heaptup->t_len,
   			InvalidBuffer, options, bistate, &vmbuffer, NULL);
   
   	CheckForSerializableConflictIn(relation, NULL, InvalidBlockNumber);
   
   	/* NO EREPORT(ERROR) from here till changes are logged */
   	START_CRIT_SECTION();
   
   	// 4. 已有插入元组tuple和存放元组的缓冲区
   	// 	  将新元组插入选中的缓冲区
   	RelationPutHeapTuple(relation, buffer, heaptup,
   						 (options & HEAP_INSERT_SPECULATIVE) != 0);
   
   	if (PageIsAllVisible(BufferGetPage(buffer))) {
   		all_visible_cleared = true;
   		PageClearAllVisible(BufferGetPage(buffer));
   		visibilitymap_clear(relation, ItemPointerGetBlockNumber(
               &(heaptup->t_self)), vmbuffer, VISIBILITYMAP_VALID_BITS);
   	}
   
   
   	// 5. 将缓冲区置为脏
   	MarkBufferDirty(buffer);
   
   	// 6. 写入Xlog日志
   	if (RelationNeedsWAL(relation)) {
   		xl_heap_insert xlrec;
   		xl_heap_header xlhdr;
   		XLogRecPtr	recptr;
   		Page		page = BufferGetPage(buffer);
   		uint8		info = XLOG_HEAP_INSERT;
   		int			bufflags = 0;
   
   		/*
   		 * If this is a catalog, we need to transmit combo CIDs to properly
   		 * decode, so log that as well.
   		 */
   		if (RelationIsAccessibleInLogicalDecoding(relation))
   			log_heap_new_cid(relation, heaptup);
   
   		/*
   		 * If this is the single and first tuple on page, we can reinit the
   		 * page instead of restoring the whole thing.  Set flag, and hide
   		 * buffer references from XLogInsert.
   		 */
   		if (ItemPointerGetOffsetNumber(&(heaptup->t_self)) == FirstOffsetNumber &&
   			PageGetMaxOffsetNumber(page) == FirstOffsetNumber)
   		{
   			info |= XLOG_HEAP_INIT_PAGE;
   			bufflags |= REGBUF_WILL_INIT;
   		}
   
   		xlrec.offnum = ItemPointerGetOffsetNumber(&heaptup->t_self);
   		xlrec.flags = 0;
   		if (all_visible_cleared)
   			xlrec.flags |= XLH_INSERT_ALL_VISIBLE_CLEARED;
   		if (options & HEAP_INSERT_SPECULATIVE)
   			xlrec.flags |= XLH_INSERT_IS_SPECULATIVE;
   
   
   		/*
   		 * For logical decoding, we need the tuple even if we're doing a full
   		 * page write, so make sure it's included even if we take a full-page
   		 * image. (XXX We could alternatively store a pointer into the FPW).
   		 */
   		if (RelationIsLogicallyLogged(relation) &&
   			!(options & HEAP_INSERT_NO_LOGICAL))
   		{
   			xlrec.flags |= XLH_INSERT_CONTAINS_NEW_TUPLE;
   			bufflags |= REGBUF_KEEP_DATA;
   
   			if (IsToastRelation(relation))
   				xlrec.flags |= XLH_INSERT_ON_TOAST_RELATION;
   		}
   
   		XLogBeginInsert();
   		XLogRegisterData((char *) &xlrec, SizeOfHeapInsert);
   
   		xlhdr.t_infomask2 = heaptup->t_data->t_infomask2;
   		xlhdr.t_infomask = heaptup->t_data->t_infomask;
   		xlhdr.t_hoff = heaptup->t_data->t_hoff;
   
   		/*
   		 * note we mark xlhdr as belonging to buffer; if XLogInsert decides to
   		 * write the whole page to the xlog, we don't need to store
   		 * xl_heap_header in the xlog.
   		 */
   		XLogRegisterBuffer(0, buffer, REGBUF_STANDARD | bufflags);
   		XLogRegisterBufData(0, (char *) &xlhdr, SizeOfHeapHeader);
   		/* PG73FORMAT: write bitmap [+ padding] [+ oid] + data */
   		XLogRegisterBufData(0,
   							(char *) heaptup->t_data + SizeofHeapTupleHeader,
   							heaptup->t_len - SizeofHeapTupleHeader);
   
   		/* filtering by origin on a row level is much more efficient */
   		XLogSetRecordFlags(XLOG_INCLUDE_ORIGIN);
   
   		recptr = XLogInsert(RM_HEAP_ID, info);
   
   		PageSetLSN(page, recptr);
   	}
   
   	END_CRIT_SECTION();
   
       // 7. 缓冲区解锁
   	UnlockReleaseBuffer(buffer);
   	if (vmbuffer != InvalidBuffer)
   		ReleaseBuffer(vmbuffer);
   
   	/*
   	 * If tuple is cachable, mark it for invalidation from the caches in case
   	 * we abort.  Note it is OK to do this after releasing the buffer, because
   	 * the heaptup data structure is all in local memory, not in the shared
   	 * buffer.
   	 */
   	CacheInvalidateHeapTuple(relation, heaptup, NULL);
   
   	/* Note: speculative insertions are counted too, even if aborted later */
   	pgstat_count_heap_insert(relation, 1);
   
   	/*
   	 * If heaptup is a private copy, release it.  Don't forget to copy t_self
   	 * back to the caller's image, too.
   	 */
   	if (heaptup != tup) {
   		tup->t_self = heaptup->t_self;
   		heap_freetuple(heaptup);
   	}
   }
   ```

   ```c
   static HeapTuple heap_prepare_insert(Relation relation, HeapTuple tup,
   		TransactionId xid, CommandId cid, int options){
   	if (IsParallelWorker())
   		ereport(ERROR,(errcode(ERRCODE_INVALID_TRANSACTION_STATE),
   				 errmsg("cannot insert tuples in a parallel worker")));
   
   	tup->t_data->t_infomask &= ~(HEAP_XACT_MASK);
   	tup->t_data->t_infomask2 &= ~(HEAP2_XACT_MASK);
   	tup->t_data->t_infomask |= HEAP_XMAX_INVALID;
   	// 设置t_xmin、t_cmin(当前command id)和t_xmax(0，无效)
   	HeapTupleHeaderSetXmin(tup->t_data, xid);
   	if (options & HEAP_INSERT_FROZEN)
   		HeapTupleHeaderSetXminFrozen(tup->t_data);
   	HeapTupleHeaderSetCmin(tup->t_data, cid);
   	HeapTupleHeaderSetXmax(tup->t_data, 0);
   	// 设置tableOid(包含此元组的表的OID)
   	tup->t_tableOid = RelationGetRelid(relation);
   
   	/* If the new tuple is too big for storage or contains already toasted
   	 * out-of-line attributes from some other relation, invoke the toaster.
   	 */
   	if (relation->rd_rel->relkind != RELKIND_RELATION &&
   		relation->rd_rel->relkind != RELKIND_MATVIEW) {
   		/* toast table entries should never be recursively toasted */
   		Assert(!HeapTupleHasExternal(tup));
   		return tup;
   	} else if (HeapTupleHasExternal(tup) || tup->t_len > TOAST_TUPLE_THRESHOLD)
   		// TOST线外存储或元组太大，通过TOAST机制执行插入操作
   		return heap_toast_insert_or_update(relation, tup, NULL, options);
   	else
   		return tup;
   }
   ```

   



### 删除元组



### 更新元组

元组更新实际上是删除和插入操作的结合，即标记删除旧元组，再插入新元组，元组更新由`heap_update`实现。



Free Space Map： 记录追踪每个(blk 或 page)的空闲空间的信息，文件的命名形式 xxx_fsm, 可以通过 extension pg_freespacemap 查看具体的详细信息



visibility map

The visibility map stores two bits per heap page。 visibility map为每一个 page存储一个2 bit 的标记

1. `VISIBILITYMAP_ALL_VISIBLE`，如果一个page 中所有的 tuple 都是 all visible 的， 那么进行标记
2. ` VISIBILITYMAP_ALL_FROZEN`，如果一个page 中所有的 tuple 都是 forzen 的, 那么进行标记

其文件的命名形式` xxx_vm`, 可以通过` extension pg_visibility `查看具体的详细信息。其有两大用途：

1. 触发Index only scan 的访问，加快SQL 执行效率
2. 加速 vacuum 的执行效率

Visibility map bits are only set by vacuum, but are cleared by any data-modifying operations on a page.

```c
// src/include/access/visibilitymapdefs.h
#define VISIBILITYMAP_ALL_VISIBLE	0x01
#define VISIBILITYMAP_ALL_FROZEN	0x02
#define VISIBILITYMAP_VALID_BITS	0x03
```

```c
uint8
visibilitymap_get_status(Relation rel, BlockNumber heapBlk, Buffer *vmbuf)
```



