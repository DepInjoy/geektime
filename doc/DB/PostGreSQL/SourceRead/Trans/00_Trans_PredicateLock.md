```plantuml
@startuml

@enduml
```

```C++
CreateSharedMemoryAndSemaphores {
    InitPredicateLocks();
}

```
```C++
void InitPredicateLocks(void)
```
src\backend\storage\lmgr\predicate.c


```C++
void RegisterPredicateLockingXid(TransactionId xid)
```

