|配置参数|默认值|描述|
|:--:|:--:|:--:|
|resource.detect-hardware-capabilities|false|是否使能硬件容量检测|

# 节点资源表示方式
`org.apache.hadoop.yarn.server.nodemanager.util.NodeManagerHardwareUtils`
```java
public class NodeManagerHardwareUtils {
    // getContainerMemoryMB：获取可用内存
    // getVCores: 获取VCore
    public static Resource getNodeResources(Configuration configuration)

}
```