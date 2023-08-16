```C++
// src/backend/utils/resowner/resowner.c
typedef struct ResourceOwnerData {
	ResourceOwner parent;		/* NULL if no parent (toplevel owner) */
	ResourceOwner firstchild;	/* head of linked list of children */
	ResourceOwner nextchild;	/* next child of same parent */
	const char *name;			/* name (just for debugging) */

	/* We have built-in support for remembering: */
	ResourceArray bufferarr;	/* owned buffers */
	ResourceArray catrefarr;	/* catcache references */
	ResourceArray catlistrefarr;	/* catcache-list pins */
	ResourceArray relrefarr;	/* relcache references */
	ResourceArray planrefarr;	/* plancache references */
	ResourceArray tupdescarr;	/* tupdesc references */
	ResourceArray snapshotarr;	/* snapshot references */
	ResourceArray filearr;		/* open temporary files */
	ResourceArray dsmarr;		/* dynamic shmem segments */
	ResourceArray jitarr;		/* JIT contexts */
	ResourceArray cryptohasharr;	/* cryptohash contexts */
	ResourceArray hmacarr;		/* HMAC contexts */

	/* We can remember up to MAX_RESOWNER_LOCKS references to local locks. */
	int			nlocks;			/* number of owned locks */
	LOCALLOCK  *locks[MAX_RESOWNER_LOCKS];	/* list of owned locks */
}			ResourceOwnerData;
```

```C++

ResourceOwner ResourceOwnerCreate(ResourceOwner parent, const char *name) {

}
```

```C++
// Create an empty ResourceOwner
ResourceOwner ResourceOwnerCreate(ResourceOwner parent, const char *name) {
    ResourceOwner owner;
    owner = (ResourceOwner) MemoryContextAllocZero(TopMemoryContext,
            sizeof(ResourceOwnerData));
    owner->name = name;

    if (parent) {
        owner->parent = parent;
        owner->nextchild = parent->firstchild;
        // parent的firstChild是新创建的ResourceOwner
        parent->firstchild = owner;
    }

	ResourceArrayInit(&(owner->bufferarr), BufferGetDatum(InvalidBuffer));
	ResourceArrayInit(&(owner->catrefarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->catlistrefarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->relrefarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->planrefarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->tupdescarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->snapshotarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->filearr), FileGetDatum(-1));
	ResourceArrayInit(&(owner->dsmarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->jitarr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->cryptohasharr), PointerGetDatum(NULL));
	ResourceArrayInit(&(owner->hmacarr), PointerGetDatum(NULL));

	return owner;
}
```