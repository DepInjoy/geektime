```C++
dispatch_command
	mysql_parse
	lex_start
	parse_sql
		THD::sql_parser
    		MYSQLparse(this, &root)
    		LEX::make_sql_cmd

    mysql_rewrite_query
	mysql_execute_command
		ex->m_sql_cmd->execute(thd)
	
Sql_cmd_dml::execute
	Sql_cmd_dml::prepare
		Sql_cmd_select::prepare_inner
			SELECT_LEX::prepare
				SELECT_LEX::resolve_subquery
					SELECT_LEX::resolve_subquery

	lock_tables

	Sql_cmd_dml::execute_inner
		SELECT_LEX_UNIT::optimize
			SELECT_LEX::optimize
				SELECT_LEX::optimize
					new JOIN
					JOIN::optimize
```