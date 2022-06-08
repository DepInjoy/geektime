整个进程的入口在`src\main\noisepage.cpp`的`int main(int argc, char *argv[])`并通过`noisepage::DBMain::Builder()`将创建主要的服务，之后调用`Run`开始运行整个服务。实现主体：
```C++
  auto db_main = noisepage::DBMain::Builder()
                     .SetSettingsParameterMap(std::move(param_map))
                     .SetUseSettingsManager(true)
                     .SetUseGC(true)
                     .SetUseCatalog(true)
                     .SetUseGCThread(true)
                     .SetUseStatsStorage(true)
                     .SetUseExecution(true)
                     .SetUseTrafficCop(true)
                     .SetUseNetwork(true)
                     .Build();

  db_main_handler_ptr = db_main.get();

  db_main->Run();
```