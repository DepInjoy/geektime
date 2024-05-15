`NereidsPlanner.plan`调用`NereidsPlanner.analyze`进行表和列解析并实现语义分析。
```java
public Plan plan(LogicalPlan plan, PhysicalProperties
        requireProperties, ExplainLevel explainLevel) {
            ......
    try (Lock lock = new Lock(plan, cascadesContext)) {
        // 2. analyze this query
        analyze();
        ......
        return physicalPlan;
    }
}

private void analyze(boolean showPlanProcess) {
    // 实际执行Analyzer.analyze
    keepOrShowPlanProcess(showPlanProcess, 
        () -> cascadesContext.newAnalyzer().analyze());
}
```
可见`Analyzer.analyze`负责完成Analyze。

