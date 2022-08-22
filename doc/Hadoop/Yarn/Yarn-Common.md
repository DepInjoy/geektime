# 服务库和事件库
## 服务库
```java
package org.apache.hadoop.service;
```
相关实现位于`ohadoop-common-project/hadoop-common/src/main/java/org/apache/hadoop/service/`

```java
public interface Service extends Closeable {
        ......
public enum STATE {
    /** Constructed but not initialized */
    NOTINITED(0, "NOTINITED"),

    /** Initialized but not started or stopped */
    INITED(1, "INITED"),

    /** started and not stopped */
    STARTED(2, "STARTED"),

    /** stopped. No further state transitions are permitted */
    STOPPED(3, "STOPPED");
}

}
```

```mermaid
classDiagram
CompositeServiceShutdownHook <|.. Runnable
CompositeServiceShutdownHook *-- CompositeService
CompositeService <|-- AbstractService
Service <|.. AbstractService
AbstractService *-- ServiceListeners
ServiceListeners *-- ServiceStateChangeListener
AbstractService *-- ServiceStateModel 

class CompositeServiceShutdownHook {
    -CompositeService compositeService;
    +void run()
}
class CompositeService {
    -List<Service> serviceList
    +void addService(Service service)
    +boolean addIfService(Object object)
    +boolean removeService(Service service)
}

class Service {
    <<interface>>
    +void init(Configuration config)
    +void start()
    +void stop()
    +void registerServiceListener(ServiceStateChangeListener listener)
    +void unregisterServiceListener(ServiceStateChangeListener listener)
    +STATE getServiceState()
}

class AbstractService {
    <<interface>>
    -final ServiceStateModel stateModel;
    -Configuration config
    -ServiceOperations.ServiceListeners listeners
    #void serviceInit(Configuration conf) throws Exception
    #void serviceStart() throws Exception 
    #void serviceStop() throws Exception
    #void registerServiceListener(ServiceStateChangeListener l)
    #void unregisterServiceListener(ServiceStateChangeListener l)
}

class ServiceListeners {
    -List<ServiceStateChangeListener> listeners
    +synchronized void add(ServiceStateChangeListener l)
    +synchronized boolean remove(ServiceStateChangeListener l)
    +synchronized void reset()
    +void notifyListeners(Service service)
}

class ServiceStateChangeListener {
    <<interface>>
    +void stateChanged(Service service)
}
class ServiceStateModel {
    -boolean[][] statemap
    -Service.STATE state
    +Service.STATE getState()
    +boolean isInState(Service.STATE proposed)
    +void ensureCurrentState(Service.STATE expectedState)
    +Service.STATE enterState(Service.STATE proposed)
    +void checkStateTransition(String name,Service.STATE state,Service.STATE proposed)
    +boolean isValidStateTransition(Service.STATE current,Service.STATE proposed)
}
```
## 事件库
`package org.apache.hadoop.yarn.event`相关实现位于`hadoop-yarn-project/hadoop-yarn/hadoop-yarn-common/src/main/java/org/apache/hadoop/yarn/event`

```mermaid
classDiagram
MultiListenerHandler --|> EventHandler
GenericEventHandler --|> EventHandler
EventHandler --* AsyncDispatcher
Event "*" --* "1" AsyncDispatcher
AsyncDispatcher --|> AbstractService
AsyncDispatcher ..|> Dispatcher
AbstractEvent --|> Event

class AsyncDispatcher {
    -BlockingQueue<Event> eventQueue
    -EventHandler<Event> handlerInstance
    #Map<Class<? extends Enum>, EventHandler> eventDispatchers
    #void dispatch(Event event)
}

class Dispatcher {
    <<interface>>
    +EventHandler<Event> getEventHandler()
    +void register(Class<? extends Enum> eventType, EventHandler handler)
}

class AbstractEvent {
    - final TYPE type;
    - final long timestamp
}

class Event {
    <<interface>>
    +TYPE getType();
    +long getTimestamp();
    +String toString();
}

class EventHandler{
    +void handle(T event)
}

class MultiListenerHandler {
    
}
```
