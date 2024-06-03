```java
// 采用反射创建调度器
Class<?> schedulerClazz = Class.forName(schedulerClassName);
if (ResourceScheduler.class.isAssignableFrom(schedulerClazz)) {
      return (ResourceScheduler) ReflectionUtils.newInstance(
      schedulerClazz,this.conf);
} else {
      throw new YarnRuntimeException("Class: " + schedulerClassName
      + " not instance of " + ResourceScheduler.class.getCanonicalName());
}
```

