# 统计信息


# 代价模型

MySQL代价模型将操作分为 Server 层和 Engine(存储引擎)层两类，Server 层主要是CPU代价，Engine层主要是 IO 代价。Mysql内置了一些代价常量，用于代价估计的计算，相关代价常量可以通过`mysql.server_cost`和`mysql.engine_cost`进行查询和修改。
```sql
-- 读取server_cost和engine_cost的代价常量
select * from mysql.server_cost;
select * from mysql.engine_cost;

-- 修改io_block_read_cost值为2
UPDATE mysql.engine_cost
  SET cost_value = 2.0
  WHERE cost_name = 'io_block_read_cost';

-- 生效，只对新连接有效，老连接无效
-- 通知服务器重写读取cost table
FLUSH OPTIMIZER_COSTS;
```

`mysql.server_cost`存储的是Server层的操作的代价估计值。

| cost name                      | 默认值 | 描述                                             |
| ------------------------------ | ------ | ------------------------------------------------ |
| `disk_temptable_create_cost`   | 20     | 基于存储引擎在磁盘创建内部临时表的代价。         |
| `disk_temptable_row_cost`      | 0.5    | 基于存储引擎使用内部临时表检索或存储一行的代价。 |
| `key_compare_cost`             | 0.05   | 比较两个记录键的代价                             |
| `memory_temptable_create_cost` | 1      | 在内存中创建临时表的代价                         |
| `memory_temptable_row_cost`    | 0.1    | 在内存中临时表检索或存储一行的代价               |
| `row_evaluate_cost`            | 0.1    | 计算条件查询的行代价                             |



`mysql.engine_cost`存储的是特定存储引擎相关操作的代价估计值。

| cost name                | 默认值 | 描述                                 |
| ------------------------ | ------ | ------------------------------------ |
| `io_block_read_cost`     | 1      | 从磁盘读取一个数据页的代价           |
| `memory_block_read_cost` | 0.25   | 从buffer pool随机读取一个block的代价 |


```C++
// 相关文件:
//		sql/opt_costconstants.h
//		sql/opt_costconstants.cc

// Server层操作代价估计常量
class Server_cost_constants {
private:
  // 定义常量的默认值
  static const double ROW_EVALUATE_COST;
  static const double KEY_COMPARE_COST;
  static const double MEMORY_TEMPTABLE_CREATE_COST;
  static const double MEMORY_TEMPTABLE_ROW_COST;
  static const double DISK_TEMPTABLE_CREATE_COST;
  static const double DISK_TEMPTABLE_ROW_COST;
 
  // Server操作的代价估计常量
  double m_row_evaluate_cost;
  double m_key_compare_cost;
  double m_memory_temptable_create_cost;
  double m_memory_temptable_row_cost;
  double m_disk_temptable_create_cost;
  double m_disk_temptable_row_cost;
};

// 存储引擎层操作代价估计常量
class SE_cost_constants {
private:
  // 代价常量的默认值
  static const double MEMORY_BLOCK_READ_COST;
  static const double IO_BLOCK_READ_COST;
  
  // 存储引擎层操作的代价估计常量
  double m_memory_block_read_cost;
  double m_io_block_read_cost;
}

Cost_model_constants {
public:
  const Server_cost_constants *get_server_cost_constants() const;
  const SE_cost_constants *get_se_cost_constants(const TABLE *table) const;
  cost_constant_error update_server_cost_constant(
      const LEX_CSTRING &name, double value);

  cost_constant_error update_engine_cost_constant(THD *thd,
          const LEX_CSTRING &se_name, uint storage_category,
          const LEX_CSTRING &name, double value);

private:
  Server_cost_constants m_server_constants;
  Prealloced_array<Cost_model_se_info, 15> m_engines;
}

class Cost_constant_cache {
public:
  void init();
  void close();
  void reload();
  const Cost_model_constants *get_cost_constants()
private:
  Cost_model_constants *current_cost_constants;
  mysql_mutex_t LOCK_cost_const;
}
```
```plantuml
class Cost_constant_cache {
  + void init();
  + void close();
  + void reload();
  + get_cost_constants() : const Cost_model_constants *

  - current_cost_constants : Cost_model_constants*
  - LOCK_cost_const : mysql_mutex_t
}

class Cost_model_constants {
  + get_server_cost_constants() const : const Server_cost_constants *;
  + get_se_cost_constants(const TABLE *table) const : const SE_cost_constants *

  + update_server_cost_constant(const LEX_CSTRING &name, double value) : cost_constant_error
  + update_engine_cost_constant(THD *thd, const LEX_CSTRING &se_name,
      uint storage_category, const LEX_CSTRING &name, double value) : cost_constant_error
}

Cost_model_constants -up-* Cost_constant_cache
Server_cost_constants -up-* Cost_model_constants
Cost_model_se_info -up-* Cost_model_constants
```

```C++
class Cost_model_server {
public:
  
private:
  const Cost_model_constants *m_cost_constants;

protected:
  const Server_cost_constants *m_server_cost_constants;
  bool m_initialized;
};

// API for getting cost estimates for operations on table data
class Cost_model_table {
protected:
  const Cost_model_server *m_cost_model_server;
  const SE_cost_constants *m_se_cost_constants;
  bool m_initialized;
private:
  const TABLE *m_table;
};
```

```C++
// 从table中随机读pages页的代价
double Cost_model_table::page_read_cost(double pages) const {
  const double in_mem = m_table->file->table_in_memory_estimate();
  const double pages_in_mem = pages * in_mem;
  const double pages_on_disk = pages - pages_in_mem;

  const double cost = buffer_block_read_cost(pages_in_mem) +
      io_block_read_cost(pages_on_disk);

  return cost;
}

// 从index中随机读取pages页的代价
double Cost_model_table::page_read_cost_index(uint index, double pages) const {
  // 根据buffer pool大小和索引大小来估算page in memory和in disk的比例
  double in_mem = m_table->file->index_in_memory_estimate(index);
  const double pages_in_mem = pages * in_mem;
  const double pages_on_disk = pages - pages_in_mem;

  // 计算读一个page的代价
  const double cost = buffer_block_read_cost(pages_in_mem) +
      io_block_read_cost(pages_on_disk);

  return cost;
}
```
代价估算`Cost_estimate`表示，并为其重构了+，-和比较运算(对total_cost进行比较)。
```plantuml
class Cost_estimate {
- double io_cost;      // cost of I/O operations
- double cpu_cost;     // cost of CPU operations
- double mem_cost;     // memory used (bytes)
- double import_cost   // cost of remote operations

+ double total_cost() const
}

note top : mem_cost未计入total_cost
```

```C++
// sql/handler.h
class Cost_estimate {
private:
  double io_cost;      // cost of I/O operations
  double cpu_cost;     // cost of CPU operations
  double import_cost;  // cost of remote operations
  double mem_cost;     // memory used (bytes)

public:
  double total_cost() const {
      return io_cost + cpu_cost + import_cost; 
  }
  double get_io_cost() const { return io_cost; }
  double get_cpu_cost() const { return cpu_cost; }
  double get_import_cost() const { return import_cost; }
  double get_mem_cost() const { return mem_cost; }

  bool operator>(const Cost_estimate &other) const {
    return total_cost() > other.total_cost() ? true : false;
  }
};
```

内存代价通过`get_disk_sweep_mrr_cost`接口(用于实现磁盘读取数据计数MRR，multi range read)调用`add_mem`进行。
```C++
bool DsMrr_impl::get_disk_sweep_mrr_cost(uint keynr, ha_rows rows,
    uint flags, uint *buffer_size, Cost_estimate *cost) {
  
}
```



```C++
class handler {
protected:
  TABLE *table;

public:
  ha_statistics stats;
};

// 存储层表对象
struct TABLE {
  handler *file{nullptr};
public:
  // Initialize the optimizer cost model
  void init_cost_model(const Cost_model_server *cost_model_server) {
    m_cost_model.init(cost_model_server, this);
  }
private:
  Cost_model_table m_cost_model;
};
```

```plantuml
class handler {
  # TABLE *table
  + ha_statistics stats;

  + Cost_estimate table_scan_cost()
  + Cost_estimate index_scan_cost(uint index, double rows)
  + Cost_estimate read_cost(uint index, double ranges, double rows)
}

struct Table {
  + handler *file
  - Cost_model_table m_cost_model

  + void init_cost_model(const Cost_model_server *cost_model_server)
  + const Cost_model_table *cost_model()
}
note top : 存储层表对象 

class Cost_model_table {
# const Cost_model_server *m_cost_model_server
# const SE_cost_constants *m_se_cost_constants

+ double page_read_cost(double pages)
+ double page_read_cost_index(uint index, double pages)
}

class Cost_model_server {
  + double row_evaluate_cost(double rows) const
}

Table -left-- handler
Cost_model_table -up-* Table

Cost_model_server -up-* Cost_model_table
SE_cost_constants -up-* Cost_model_table
```



| 接口                                                         | 描述                                                         |
| :----------------------------------------------------------- | ------------------------------------------------------------ |
| `virtual Cost_estimate table_scan_cost();`                   | Cost estimate for doing a complete table scan.</br>全表扫描的代价估计 |
| `virtual Cost_estimate index_scan_cost(uint index, double ranges, double rows);` | Cost estimate for reading a number of ranges from an index.<br/>  |
| `virtual Cost_estimate read_cost(uint index, double ranges, double rows);` | Cost estimate for reading a set of ranges from the table using an index to access it.<br/> |




```C++
// sql/handler.cc

virtual double scan_time() {
  return ulonglong2double(stats.data_file_length) / IO_SIZE + 2;
}

Cost_estimate handler::table_scan_cost() {
  // scan_time计算数据所占page数
  // scan cost = pages * 读取1个page的cost
  const double io_cost = scan_time() * table->cost_model()->page_read_cost(1.0);
  Cost_estimate cost;
  cost.add_io(io_cost);
  return cost;
}

/*----------------------- */

double handler::index_only_read_time(uint keynr, double records) {
  double read_time;
  uint keys_per_block = (stats.block_size / 2 /
           (table_share->key_info[keynr].key_length + ref_length) + 1);
  read_time = ((double)(records + keys_per_block - 1) / (double)keys_per_block);
  return read_time;
}

Cost_estimate handler::index_scan_cost(uint index,
       double ranges MY_ATTRIBUTE((unused)), double rows) {
  // 估计index占Page的个数
  const double io_cost = index_only_read_time(index, rows) *
        table->cost_model()->page_read_cost_index(index, 1.0);
  Cost_estimate cost;
  cost.add_io(io_cost);
  return cost;
}

/*----------------------------------------------------*/
Cost_estimate handler::read_cost(uint index, double ranges, double rows) {
  const double io_cost = read_time(index, static_cast<uint>(ranges), 
       static_cast<ha_rows>(rows)) * table->cost_model()->page_read_cost(1.0);
  Cost_estimate cost;
  cost.add_io(io_cost);
  return cost;
}
```
