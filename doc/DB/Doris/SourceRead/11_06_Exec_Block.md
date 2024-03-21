```C++
using Container = ColumnsWithTypeAndName;
using ColumnsWithTypeAndName = std::vector<ColumnWithTypeAndName>

using IndexByName = phmap::flat_hash_map<String, size_t>;
```

```plantuml
@startuml
class Block {
    - Container data
    - IndexByName index_by_name \n\t<column name, column index> Hash Map
    - std::vector<bool> row_s
}
note top : 1. Block对象的本质是一组由数据对象(Column), \n数据类型(DataType)和列名称(列名称字符串)\n组成的三元组(ColumnWithTypeAndName)\n\n 2. IndexByName是Hash Map可以根据列名寻找列index

struct ColumnWithTypeAndName {
    + ColumnPtr column
    + DataTypePtr type
    + String name
}

interface IColumn {
    + virtual int compare_at(size_t n, size_t m,\n\tconst IColumn& rhs,\n\tint nan_direction_hint) const = 0
}

interface IDataType {
    + MutableColumnPtr create_column() const = 0
}

ColumnWithTypeAndName -up-o Block
IColumn -up-o ColumnWithTypeAndName
IDataType -up-o ColumnWithTypeAndName
@enduml
```

```plantuml
@startuml

@enduml
```

```C++
class IColumn : public COW<IColumn> {
public:
    // Compares (*this)[n] and rhs[m]
    virtual int compare_at(size_t n, size_t m, const IColumn& rhs,
                        int nan_direction_hint) const = 0;
}
```

# 使用内存信息

```plantuml
@startuml
class Block {
    - Container data
    + size_t Block::bytes() const
    + size_t Block::allocated_bytes() const
}

struct ColumnWithTypeAndName {
    + ColumnPtr column
    + DataTypePtr type
    + String name
}

interface IColumn {
    + virtual size_t byte_size() const = 0
    + virtual size_t allocated_bytes() const = 0
}
note bottom : allocated_bytes >= byte_size 

ColumnWithTypeAndName -up-o Block : 列存\n<1, N>
IColumn -up-o ColumnWithTypeAndName
@enduml
```