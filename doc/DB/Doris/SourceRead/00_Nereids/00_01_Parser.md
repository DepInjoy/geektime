`NereidsParser`根据传入的SQL语句经过词法和语法分析生成抽象语法树，并借助`LogicalPlanBuilder`生成逻辑计划。
```java
public class NereidsParser {
    private static final ParseErrorListener PARSE_ERROR_LISTENER 
            = new ParseErrorListener();
    private static final PostProcessor POST_PROCESSOR =
            new PostProcessor();

    // 对外提供的将SQL语句转化为AST和Logical Plan的接口
    public List<StatementBase> parseSQL(String originStr) {
        return parseSQL(originStr, (LogicalPlanBuilder) null);
    }
            ......
}
```
`NereidsParser.parseSQL`是对外提供的将SQL语句转化为AST和Logical Plan的接口，它的主要调用流程：
```java
NereidsParser.parseSQL
    NereidsParser.parseMultiple
        NereidsParser.parse // 主实现,生成AST并转化为LogicalPlan
```
`NereidsParser.parse`是生成AST和Logical Plan的主要实现接口，下面看下它的实现：
```java
// NereidsParser
private <T> T parse(String sql, 
        @Nullable LogicalPlanBuilder logicalPlanBuilder,
        Function<DorisParser, ParserRuleContext> parseFunction) {
    // 1. 词法和语法分析，生成抽象语法树(AST)
    ParserRuleContext tree = toAst(sql, parseFunction);

    // 2. 采用visit模式将AST转化逻辑计划
    LogicalPlanBuilder realLogicalPlanBuilder = logicalPlanBuilder == null
                ? new LogicalPlanBuilder() : logicalPlanBuilder;
    return (T) realLogicalPlanBuilder.visit(tree);
}
```

对于`toAst`接口实现：
```java
private ParserRuleContext toAst(String sql,
        Function<DorisParser, ParserRuleContext> parseFunction) {
    // 1. 词法分析
    DorisLexer lexer = new DorisLexer(new CaseInsensitiveStream(CharStreams.fromString(sql)));
    CommonTokenStream tokenStream = new CommonTokenStream(lexer);

    // 2. 语法分析
    DorisParser parser = new DorisParser(tokenStream);

    parser.addParseListener(POST_PROCESSOR);
    parser.removeErrorListeners();
    parser.addErrorListener(PARSE_ERROR_LISTENER);

    ParserRuleContext tree;
    try {
        // 2.1 先尝试SLL模式解析
        parser.getInterpreter().setPredictionMode(PredictionMode.SLL);
        tree = parseFunction.apply(parser);
    } catch (ParseCancellationException ex) {
        // 2.2 解析失败，尝试用LL模式解析
        tokenStream.seek(0); 
        parser.reset();
        parser.getInterpreter().setPredictionMode(PredictionMode.LL);
        tree = parseFunction.apply(parser);
    }
    return tree;
}
```