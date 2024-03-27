```C
ProcessQuery
    queryDesc->plannedstmt->query_mem = ResourceManagerGetQueryMemoryLimit
```

```c
/* src/backend/utils/resource_manager/memquota.c
 *
 * Calculate the amount of memory reserved for the query
 * 
 * 分资源组和资源队列两种情况
 *	1. 资源队列
 *	2. 资源组，借助ResourceGroupGetQueryMemoryLimit来实现
 */
int64 ResourceManagerGetQueryMemoryLimit(PlannedStmt* stmt) {
    if (Gp_role != GP_ROLE_DISPATCH) return stmt->query_mem;

	/* no limits in single user mode. */
	if (!IsUnderPostmaster) return 0;

	if (IsResQueueEnabled())
		return ResourceQueueGetQueryMemoryLimit(stmt, ActivePortal->queueId);
	if (IsResGroupActivated())
		return ResourceGroupGetQueryMemoryLimit();

	return 0;
}
```

