```java
 // abstract class for all rule factories build one rule used in rewrite stage.
public abstract class OneRewriteRuleFactory
        extends OnePlanRuleFactory implements RewriteRuleFactory {
}

/**
 * abstract class for all rule factories build one plan rule.
 */
public abstract class OnePlanRuleFactory extends OneRuleFactory implements PlanRuleFactory {
}

/**
 * interface for all plan rule factories.
 */
public interface PlanRuleFactory extends RuleFactory {
}

/**
 * interface for all rule factories for build some rules.
 */
public interface RuleFactory extends Patterns {
    // need implement
    List<Rule> buildRules();

    // need implement
    @Override
    RulePromise defaultPromise();
}
```
```java
/**
 * Abstract class for all rules.
 */
public abstract class Rule {
    private final RuleType ruleType;
    private final Pattern<? extends Plan> pattern;
    private final RulePromise rulePromise;

    public abstract List<Plan> transform(Plan node, CascadesContext context) throws TransformException;

    /** callback this function when the traverse framework accept a new plan which produce by this rule */
    public void acceptPlan(Plan plan) {

    }
}