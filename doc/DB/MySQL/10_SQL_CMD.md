```C++
class Sql_cmd {
public:
  virtual bool prepare(THD *thd MY_ATTRIBUTE((unused))) {
    set_prepared();
    return false;
  }
  virtual bool execute(THD *thd) = 0;

private:
  Prepared_statement *m_owner;
  bool m_prepared;
  bool m_secondary_engine_enabled{true};
  const handlerton *m_secondary_engine{nullptr};

protected:
  bool prepare_only;	
};

// sql/sql_cmd_dml.h
class Sql_cmd_dml : public Sql_cmd {
public:
  virtual bool prepare(THD *thd);
  virtual bool execute(THD *thd);

  virtual bool precheck(THD *thd) = 0;
  virtual bool prepare_inner(THD *thd) = 0;
  virtual bool execute_inner(THD *thd);
}

class Sql_cmd_select : public Sql_cmd_dml {
protected:
  virtual bool precheck(THD *thd) override;
  virtual bool prepare_inner(THD *thd) override;
}
/**
  A base class for CREATE/ALTER TABLE commands and friends.
  Child classes deal with SQL statements:
  * ALTER TABLE
  * ANALYZE TABLE
  * CACHE INDEX
  * CHECK TABLE
  * CREATE INDEX
  * CREATE TABLE
  * DROP INDEX
  * LOAD INDEX
  * OPTIMIZE TABLE
  * REPAIR TABLE
*/
class Sql_cmd_ddl_table : public Sql_cmd;

// sql/sql_cmd_ddl.h
class Sql_cmd_ddl : public Sql_cmd;
```

```plantuml
class Sql_cmd_select {
# virtual bool precheck(THD *thd) override
# virtual bool prepare_inner(THD *thd) override
}

class Sql_cmd_dml {
+ virtual bool execute(THD *thd) overide
+ virtual bool prepare_inner(THD *thd) = 0;
+ virtual bool execute_inner(THD *thd);
}

class Sql_cmd_create_table {
    + bool execute(THD *thd) override
    + bool prepare(THD *thd) override
}

class Sql_cmd_ddl_table {
}
note right: CREATE/ALTER TABLE命令的基类

class Sql_cmd {
  + virtual bool execute(THD *thd) = 0;
  + virtual bool prepare(THD *thd);
}

Sql_cmd_select -down--|> Sql_cmd_dml
Sql_cmd_create_table --|> Sql_cmd_ddl_table
Sql_cmd_ddl_table --|> Sql_cmd
Sql_cmd_dml -right--|> Sql_cmd
```