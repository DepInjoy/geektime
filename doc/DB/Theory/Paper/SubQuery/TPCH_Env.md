```shell
cp makefile.suite Makefile

```

编辑`Makefile`

```
CC      =       gcc
DATABASE= ORACLE
MACHINE = LINUX
WORKLOAD = TPCH
```

```
make
```

```shell
# 生成1G数据
./dbgen -s 1
```



```shell
# 将测试数据**.tbl转换为postgresql识别的CSV格式，删除末尾的分隔符|
for i in `ls *.tbl`; do sed 's/|$//' $i > ${i/tbl/csv}; done

# 创建软连接链接到当前目录
ln -s `pwd` /tmp/dss-data

SF=?
mkdir dss/queries
for q in `seq 1 22`
do
    DSS_QUERY=dss/templates ./qgen -s $SF $q > dss/queries/$q.sql
    sed 's/^select/explain select/' dss/queries/$q.sql > dss/queries/$q.explain.sql
done
```

