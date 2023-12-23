Doris旧版优化器采用Java CUP Parser，语法规则定义在 `fe/fe-core/src/main/cup/sql_parser.cup`，SQL语句会被解析为抽象语法树(AST)，Token定义在`fe/fe-core/src/main/jflex/sql_scanner.flex`。Doris 2.0新版优化器采用Antlr4进行词法和语法分析。

# NereidsParser

采用Antlr4词法分析器，对应配置文件`fe/fe-core/src/main/antlr4/org/apache/doris/nereids/DorisLexer.g4`,
采用Antlr4  LL(*)语法分析器，对应配置文件`fe/fe-core/src/main/antlr4/org/apache/doris/nereids/DorisParser.g4`。

```java
public class NereidsParser {
    public List<StatementBase> parseSQL(String originStr) {
        List<Pair<LogicalPlan, StatementContext>> logicalPlans = parseMultiple(originStr);
        List<StatementBase> statementBases = Lists.newArrayList();
        for (Pair<LogicalPlan, StatementContext> parsedPlanToContext : logicalPlans) {
            statementBases.add(new LogicalPlanAdapter(parsedPlanToContext.first,
                                                      parsedPlanToContext.second));
        }
        return statementBases;
    }

    public List<Pair<LogicalPlan, StatementContext>> parseMultiple(String sql) {
        return parse(sql, DorisParser::multiStatements);
    }

    private <T> T parse(String sql, Function<DorisParser, ParserRuleContext> parseFunction) {
        // ParserRuleContext含List<ParseTree>类型的children成员表示抽象语法树
        ParserRuleContext tree = toAst(sql, parseFunction);
        // visitRegularQuerySpecification将SELECT语句转化成LogicalPlan
        // 参见fe/fe-core/src/main/antlr4/org/apache/doris/nereids/DorisParser.g4
        // querySpecification，并不了解这个语法？？？？
        LogicalPlanBuilder logicalPlanBuilder = new LogicalPlanBuilder();
        return (T) logicalPlanBuilder.visit(tree);
    }

}
```
将SELECT转换成逻辑计划和`DorisParser.g4`下面这段相关，通过`LogicalPlanBuilder`的`visitRegularQuerySpecification`来实现，目前不了解相关实现机制
```
querySpecification
    : selectClause
      fromClause?
      whereClause?
      aggClause?
      havingClause?
      {doris_legacy_SQL_syntax}? queryOrganization                         #regularQuerySpecification
    ;
```


# 参考资料
1. [ANTLR 4简明教程](https://www.bookstack.cn/read/antlr4-short-course/README.md)