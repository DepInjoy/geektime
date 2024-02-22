SQLsmith定义了一些数据结构来表达AST并提供了接口将其转换成SQL字符串，从某种意义上来说，是数据库内核Parser解析的逆操作。这个得到的SQL字符串发送给数据库执行，如果执行成功则忽略，如果执行报错可以进行一些记录和统计工作。

<center>
    <img src=./img/sqlsmith-eg-ast.png />
    <div>SQLsmith git仓库AST示例，链接地址：https://github.com/anse1/sqlsmith/blob/master/ast.png</div>
</center>
<br/>

其中`query_spec`是SELECT的语句的AST,其`out`接口将其逆转换成SQL字符串
```C++
void query_spec::out(std::ostream &out) {
    out << "select " << set_quantifier << " "
        << *select_list;
    indent(out);
    out << *from_clause;
    indent(out);
    out << "where ";
    out << *search;
    if (limit_clause.length()) {
        indent(out);
        out << limit_clause;
    }
}
```
---
[SQLsmith:随机SQL query生成器](https://github.com/anse1/sqlsmith/tree/master?tab=readme-ov-file)程序的入口在`sqlsmith.cc`

```C++
int main(int argc, char *argv[]) {
    // 生成schema类,读取目标数据库中的各种元数据(metadata)
    shared_ptr<schema> schema;
    schema = make_shared<...>(...)
    scope scope;
    schema->fill_scope(scope);

    // 格式化输出和统计
    vector<shared_ptr<logger> > loggers;
    loggers.push_back(make_shared<impedance_feedback>());

    // 数据库连接器
    shared_ptr<dut_base> dut;

    // 生成query AST
    shared_ptr<prod> gen = statement_factory(&scope);

    // 根据AST生成SQL
    ostringstream s;
    gen->out(s);

    // 执行SQL
    dut->test(s.str());
    for (auto l : loggers)
        l->executed(*gen);
}
```

主要结构类图
```plantuml
@startuml
struct scope {
    + struct scope *parent;
  /// available to table_ref productions
    + vector<named_relation*> tables;
 /// available to column_ref productions
    + vector<named_relation*> refs;
    + struct schema *schema;
  /// Counters for prefixed stmt-unique identifiers
  + shared_ptr<map<string,unsigned int> > stmt_seq
}

struct schema {
    + std::vector<sqltype *> types;

    + std::vector<table> tables;
    + std::vector<op> operators;
    + std::vector<routine> routines;
    + std::vector<routine> aggregates;

    + virtual std::string quote_name(const std::string &id) = 0
    + void fill_scope(struct scope &s)
}

struct table {
    + string schema;
    + bool is_insertable;
    + bool is_base_table;
    + vector<string> constraints
}
note right: 包含约束(constraints)和列(column)

struct relation {
    + vector<column> cols
    + virtual vector<column> &columns()
}

struct named_relation {
    + string name;
    + virtual string ident()
}

struct column {
    + string name
    + sqltype *type
}

struct sqltype {
    + string name
    + static map<string, struct sqltype*> typemap
    + static struct sqltype *get(string s)
}

struct routine {
    + string specific_name;
    + string schema;
    + vector<sqltype *> argtypes;
    + sqltype *restype;
    + string name
}

sqltype -o column
column -up-o relation
table -down-|> named_relation
named_relation -right-|> relation
routine -up-o schema
table -left-o schema
schema -up-o scope
@enduml
```


SQLsmith大致包含三大模块：
1. 抽象语法树(AST)生成器
2. 格式化输出和统计工具
3. 目标数据库连接器
其工作流程就是依次调用这三个模块：首先由AST生成器产生随机的query AST，然后由格式化工具将其转化为SQL字符串，最后由数据库连接器将该SQL字符串在目标数据库上执行。


# AST生成器

AST生成器可以进一步细分为三部分：句法(syntax)模块;语义(semantic)模块; 语法(grammar)模块。

## syntax模块
这部分包括prod(prod.cc和prod.hh)和expr(expr.cc和expr.hh)这两组文件，负责定义value expression的syntax。prod是expr的基类，prod定义了基本的抽象类和方法，expr继承prod并定义具体的表达式。如果需要扩展表达式syntax的话，通常只需要修改expr而不需要改动prod。这里只负责value expression的定义，即boolean expression，constant expression等可以作为"值"(value)来使用的表达式.

类图
```plantuml
@startuml
struct prod {
    + struct prod *pprod
    + struct scope *scope
    + int level
    + long retries
    + long retry_limit = 100

    + virtual void indent(std::ostream &out)
    + virtual void out(std::ostream &out) = 0
    + virtual void accept(prod_visitor *v) { v->visit(this); }
    + virtual void match()
    + virtual void fail(const char *reason);
}

struct funcall {
    + routine *proc;
    + bool is_aggregate;
    + vector<shared_ptr<value_expr> > parms
    + void out(std::ostream &out)
    + void accept(prod_visitor *v)
}

struct prod_visitor {
  + virtual void visit(struct prod *p) = 0
}

struct value_expr {
    + sqltype *type;
    + virtual void out(std::ostream &out) = 0
    + static shared_ptr<value_expr> factory(\n\tprod *p, sqltype *type_constraint = 0);
}

struct case_expr {
  + shared_ptr<value_expr> condition;
  + shared_ptr<value_expr> true_expr;
  + shared_ptr<value_expr> false_expr;
  + virtual void out(std::ostream &out);
  + virtual void accept(prod_visitor *v);
}

struct scope {
    + struct scope *parent;
    + vector<named_relation*> tables //table_ref
    + vector<named_relation*> refs; // column_ref
    + struct schema *schema
    + shared_ptr<map<string,unsigned int> > stmt_seq
}

struct schema {

}

prod_visitor -left--> prod : 遍历AST
value_expr -down-|> prod : 继承
case_expr -down-|> value_expr
funcall -down-|> value_expr
scope -right-o prod
schema -up-o scope
@enduml
```

一些重要接口
```C++
struct value_expr: prod {
  sqltype *type;
  virtual void out(std::ostream &out) = 0;
  virtual ~value_expr() { }
  value_expr(prod *p) : prod(p) { }
  // 根据std::uniform_int_distribution随机数生成不同的expr
  static shared_ptr<value_expr> factory(prod *p, sqltype *type_constraint = 0);
};
```

```C++
// 遍历AST的基类
struct prod_visitor {
  virtual void visit(struct prod *p) = 0;
  virtual ~prod_visitor() { }
};

// AST基类
struct prod {
    // 根据level处理换行和缩进
    virtual void indent(std::ostream &out);
    // 生成SQL
    virtual void out(std::ostream &out) = 0;

    // Check with the impedance matching code whether this production
    // has been blacklisted and throw an exception.
    virtual void match();

    // 采用visitor方式遍历AST
    virtual void accept(prod_visitor *v) { v->visit(this); }
    //生成failed to generate报错
    virtual void fail(const char *reason);
}
```



## 语义(semantic)模块
包括relmodel这一组文件，负责描述relational model，定义了table，column等主要的class以及他们之间的包含和约束关系。

```plantuml
@startuml
struct column {
    + string name;
    + sqltype *type;
}

struct relation {
  + vector<column> cols
}

struct named_relation {
  string name;
}

struct aliased_relation {
    + relation *rel
}

struct table {
    + string schema
    + bool is_insertable
    + bool is_base_table
    + vector<string> constraints
}

table -down-|> named_relation
aliased_relation -down-|> named_relation
named_relation -down-|> relation
column -o relation

@enduml
```

## 语法(grammar)模块
包括grammar这一组文件，负责定义query的syntax。定义了最高层的syntax，即整个query的syntax，比如一个select query规定为一个select clause+可选的from clause及where clause，而一个select clause定义为一个select关键字+一个或多个value expression，等等。语法模块以上述句法和语义模块为基础，是AST生成器中最顶层的模块，也是最主要的模块。如果要修改query的语法，主要就是修改这部分代码。
```C++
// 生成select。insert, update,merge等查询
shared_ptr<prod> statement_factory(struct scope *s)
```

```plantuml
@startuml
struct query_spec {
    + std::string set_quantifier;
    + shared_ptr<struct from_clause> from_clause;
    + shared_ptr<struct select_list> select_list;
    + shared_ptr<bool_expr> search;
    + std::string limit_clause;
    + struct scope myscope

    + void out(std::ostream &out)
    + void accept(prod_visitor *v)
}
note left : select查询


struct select_for_update {
    + const char *lockmode;
}

struct prepare_stmt {
    + query_spec q
    + static long seq
    + long id
}

struct prod {}

query_spec -down-|> prod
prepare_stmt -down-|> query_spec
select_for_update -down-|> query_spec
@enduml
```

```plantuml
@startuml

struct table_ref {
    + vector<shared_ptr<named_relation> > refs;
    + static shared_ptr<table_ref> factory(prod *p)
}
note bottom : factory接口根据随机数生成派生的table

struct table_subquery  {
  bool is_lateral
}

struct lateral_subquery {

}
note left: 标量子查询(is_lateral=true)

struct table_sample {
    + struct table *t
    - string method
    - double percent
}

struct table_or_query_name {
  named_relation *t;
}

struct joined_table {
    + std::string type;
    + std::string alias;
    + shared_ptr<table_ref> lhs;
    + shared_ptr<table_ref> rhs;
    + shared_ptr<join_cond> condition;
}

table_ref -right-|> prod
table_subquery -down-|> table_ref
joined_table -down-|> table_ref
table_sample -down-|> table_ref
table_or_query_name -|> table_ref : table as ref ident
lateral_subquery -down-|> table_subquery
@enduml
```

```plantuml
@startuml
struct merge_stmt {
}

struct modifying_stmt {
    + table *victim;
    + struct scope myscope
}

struct insert_stmt {

}

struct set_list {}

struct upsert_stmt {
}

struct update_stmt {

}

upsert_stmt -down-|> insert_stmt
update_stmt -down-|> modifying_stmt
insert_stmt -down-|> modifying_stmt
merge_stmt -down-|> modifying_stmt
modifying_stmt -down-|>  prod
set_list -down-|> prod : set param=
@enduml
```

# 格式化输出和统计工具
包含log，dump和impedance三组文件，负责格式化输出、AST转换为SQL字符串、以及收集并报告query生成和执行过程中的必要统计信息。

```plantuml
@startuml
struct logger {
    + virtual void generated(prod &query)
    + virtual void executed(prod &query)
    + virtual void error(prod &query, const dut::failure &e)
}

struct impedance_feedback {
    + virtual void executed(prod &query)
    + virtual void error(prod &query,\n\tconst dut::failure &e)
}
note top: 记录指向成功(occurances_in_ok_query)\n和失败(occurances_in_failed_query)的AST\n\n借助impedance_visitor实现遍历AST



struct query_dumper {}
note top : dump所有查询

struct stats_collecting_logger {
  long queries = 0;
  float sum_nodes = 0;
  float sum_height = 0;
  float sum_retries = 0;
}
note right : 收集统计信息

struct cerr_logger {
    + const int columns = 80;
  std::map<std::string, long> errors;
  virtual void report();
  virtual void generated(prod &query);
  virtual void executed(prod &query);
  virtual void error(prod &query,\n\tconst dut::failure &e);
}
note bottom : stderr logger

struct pqxx_logger {
    + long id;
    + std::shared_ptr<pqxx::connection> c
}
note bottom : logger to postgres database

struct ast_logger {
    + int queries = 0
    + virtual void generated(\n\tprod &query);
}
note top : 将AST树保存到sqlsmith-<query>.xml文件\n\n借助graphml_dumper实现AST遍历(prod_visitor)

struct query_dumper {}

pqxx_logger -up-|> stats_collecting_logger
cerr_logger -up-|> stats_collecting_logger
stats_collecting_logger -up-|> logger
query_dumper -down-|> logger
impedance_feedback -down-|> logger
ast_logger -down-|> logger
@enduml
```

# 目标数据库连接器
包括schema和dut两组文件，负责连接、读取目标数据库并将query在数据库上执行。其中schema负责描述数据库的内容和结构，比如在哪个table中可以获取data type信息、从哪个table中读取所有UDF的信息等。而dut负责调用相应的数据库驱动API来连接数据库并执行SQL指令。

```plantuml
@startuml
struct schema {
    + virtual std::string quote_name(const std::string &id) = 0;
}

struct schema_pqxx {
    + pqxx::connection c;
    + map<OID, pg_type*> oid2type;
    + map<string, pg_type*> name2type;
}

struct schema_sqlite {}
schema_pqxx -down-|> schema : 继承
schema_sqlite -down-|> schema : 继承
@enduml
```
```plantuml
@startuml
struct dut_base {
    + std::string version;
    + virtual void test(const std::string &stmt) = 0;
}
note bottom : 连接基类

struct dut_libpq {
    + PGconn *conn = 0
    + std::string conninfo_
}

struct dut_sqlite {
  virtual void test(const std::string &stmt);
}
note top: SQLite 3连接

dut_sqlite -down-|> dut_base
dut_libpq -down-|> dut_base 
@enduml
```


# 参考资料
1. [知乎：SQLsmith-开源随机SQL query生成器](https://zhuanlan.zhihu.com/p/397285187)