在`InitPostgres`中调用`GPMemoryProtect_Init()`初始化Memory Protection，

```c
/* Initializes per-process data structures and enables memory protection */
void GPMemoryProtect_Init() {
	if (NULL == MySessionState) {
		/* Only database sessions have memory protection enabled */
		return;
	}

	/*
	 * Lock in the current thread that is initializing the memory protection
	 * so that no other thread can use memory protection later on.
	 */
	memprotOwnerThread = pthread_self();
	VmemTracker_Init();
	InitPerProcessOOMTracking();
	gp_mp_inited = true;
}
```

在`InitResManager`中调用`GPMemoryProtect_TrackStartupMemory`，

```c
// Add the per-process startup committed memory to vmem tracker.
void GPMemoryProtect_TrackStartupMemory(void) {
	int64		bytes = 0;

	/*
	 * When compile without ORCA a postgress process will commit 6MB, this
	 * includes the memory allocated before vmem tracker initialization.
	 */
	bytes += 6L << BITS_IN_MB;

#ifdef USE_ORCA
	/* When compile with ORCA it will commit 6MB more */
	bytes += 6L << BITS_IN_MB;

	/*
	 * When optimizer_use_gpdb_allocators is on, at least 2MB of above will be
	 * tracked by vmem tracker later, so do not recount them.  This GUC is not
	 * available until gpdb 5.1.0 .
	 */
#if GP_VERSION_NUM >= 50100
	if (optimizer_use_gpdb_allocators)
		bytes -= 2L << BITS_IN_MB;
#endif  /* GP_VERSION_NUM */
#endif  /* USE_ORCA */

	/* Leave some buffer for extensions like metrics_collector */
	bytes += 2L << BITS_IN_MB;

	/* Register the startup memory */
	MemoryAllocationStatus status = VmemTracker_RegisterStartupMemory(bytes);
	if (status != MemoryAllocation_Success)
		gp_failed_to_alloc(status, 0, bytes);
}
```

