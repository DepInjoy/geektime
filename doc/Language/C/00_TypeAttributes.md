
在C和C++中，`__attribute__((packed))`是一个类型属性，用于指定数据结构的内存对齐方式，确保编译器在内存中紧凑地布局结构的成员，不会在成员之间插入任何填充字节以优化对齐。这对于处理硬件或网络协议等需要精确控制内存布局的情况很有用。
```C
// 不进行内存对齐,成员紧密排列, 无填充字节
// sizeof(sSampleStruct) = 8
// 如果内存对齐，sizeof(sSampleStruct) = 12
typedef struct {  
  char              Data1;
  int               Data2;
  unsigned short    Data3;
  char              Data4;
} __attribute__((packed)) sSampleStruct;
```

