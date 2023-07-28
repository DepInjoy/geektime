查询优化入口

```C++
// sql/sql_optimizer.cc
bool JOIN::optimize()
```



```C++
// 
class ha_xengine : public my_core::handler;
```



```C++
// storage/xengine/handler/ha_xengine.cc

// handlerton定义存储引擎，并包含指向函数的函数指针
handlerton *xengine_hton;

// 存储引擎实例化
static handler *xengine_create_handler(my_core::handlerton *const hton,
           my_core::TABLE_SHARE *const table_arg, bool partitioned,
           my_core::MEM_ROOT *const mem_root) {
      if (partitioned) {
        //TODO we should implement partion in the future
        my_printf_error(ER_NOT_SUPPORTED_YET,
            "Create partitioned table is not supported yet in xengine.", MYF(0));
        return nullptr;
      }
      return new (mem_root) ha_xengine(hton, table_arg);
}

// Storage Engine initialization function, invoked when plugin is loaded.
static int xengine_init_func(void *const p) {
  	xengine_hton = (handlerton *)p;
   		......
	xengine_hton->create = xengine_create_handler;
}


```

# 参考资料

1. [MySQL:第16章：编写自定义存储引擎](https://www.mysqlzh.com/doc/141.html)
2. 