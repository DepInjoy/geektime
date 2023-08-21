
## 数据查找
对SkipList中的数据进行查找的伪码
```sql
search(list, searchKey)
    x := list->header
    -- loop invariant: x→key < searchKey
    for i := list->level downto 1 do
        while x->forward[i]->key < searchKey do
            -- i表示层级,forward[i]表示节点后方的指针
            x := x->forward[i]
    -- x->key < searchKey <= x->forward[1]->key
    x := x->forward[1]
    if x->key = searchKey then return x->value
    else return failure
```
其主要的思想是：从链表头开始，从最高层依次向低层查找，同一层节点查找和普通有序表一致(顺序往后找到第一个大于或等于当前查找数值searchKey的节点)，当在本层级未找到时，就下降一个层级继续，直到找到或者发现链表中不包含searchKey为止。

## 插入数据

首先了解如何随机确定节点层级
```sql
randomLevel()
    |v| := 1
    -- random() that return a random value in[0...1)
    while random() < p and |v| MaxLevel do
        |v| := |v| + 1
    return |v|
```

插入新数据
```
```

## 删除数据
