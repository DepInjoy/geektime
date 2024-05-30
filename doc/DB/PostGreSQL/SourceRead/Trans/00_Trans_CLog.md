CLog保存事务的状态

```C++
void ExtendCLOG(TransactionId newestXact) {
	int			pageno;

	/*
	 * No work except at first XID of a page.  But beware: just after
	 * wraparound, the first XID of page zero is FirstNormalTransactionId.
	 */
	if (TransactionIdToPgIndex(newestXact) != 0 &&
		!TransactionIdEquals(newestXact, FirstNormalTransactionId))
		return;

	pageno = TransactionIdToPage(newestXact);
	LWLockAcquire(XactSLRULock, LW_EXCLUSIVE);

	/* Zero the page and make an XLOG entry about it */
	ZeroCLOGPage(pageno, true);
	LWLockRelease(XactSLRULock);
}
```

```C++
void ExtendCommitTs(TransactionId newestXact)
```

```C++
void ExtendSUBTRANS(TransactionId newestXact)
```

