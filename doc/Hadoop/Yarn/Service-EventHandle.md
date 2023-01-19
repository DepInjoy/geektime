

# 服务库

服务库的实现源码位于`hadoop-common-project\hadoop-common\src\main\java\org\apache\hadoop\service\AbstractService.java` 的`org.apache.hadoop.service`中，借助两个类来实现：

- `AbstractService`
- `CompositeService`提供组合服务，例如`ResourceManager`。

```java
public abstract class AbstractService implements Service

// 组合服务
public class CompositeService extends AbstractService
private final List<Service> serviceList = new ArrayList<Service>();
```

```java
public abstract class AbstractService implements Service {
  /**
   * Service name.
   */
  private final String name;

  /** service state */
  private final ServiceStateModel stateModel;
}
```

## 