```plantuml
@startuml
interface RewriteRuleFactory {}
interface PlanRuleFactory {}

interface RuleFactory {
    + List<Rule> buildRules();
    + RulePromise defaultPromise();
}

class Rule {
    + List<Plan> transform(Plan node, CascadesContext context)
}

RewriteRuleFactory -down.|> PlanRuleFactory
PlanRuleFactory -down.|> RuleFactory
RuleFactory -right.|> Patterns

Rule -up-* RuleFactory
@enduml
```

```java
// interface for all plan rule factories.
public interface PlanRuleFactory extends RuleFactory {
}

// interface for all rule factories for build some rules.
public interface RuleFactory extends Patterns {
    List<Rule> buildRules();

    @Override
    RulePromise defaultPromise();
}

public abstract class Rule {
    private final RuleType ruleType;
    private final Pattern<? extends Plan> pattern;
    private final RulePromise rulePromise;

    public abstract List<Plan> transform(Plan node, CascadesContext context) throws TransformException;
}
```

## Rewrite Rule

```plantuml
@startuml
class EliminateFilter implements RewriteRuleFactory {
    + List<Rule> buildRules() // @Override
}
note left of EliminateFilter : 举例, 删除True或False的Filter

interface PlanRuleFactory {}

interface RewriteRuleFactory extends PlanRuleFactory, GeneratedPlanPatterns {

}
note left of RewriteRuleFactory : interface for all \n rewrite rule factories
@enduml
```

```java
// interface for all rewrite rule factories.
public interface RewriteRuleFactory extends PlanRuleFactory, GeneratedPlanPatterns {
    @Override
    default RulePromise defaultPromise() {
        return RulePromise.REWRITE;
    }
}
```