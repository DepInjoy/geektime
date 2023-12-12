Doris后端(be)进程的main函数实现在`be/src/service/doris_main.cpp`
# 初始化ExecEnv
```C++
// init exec env
auto exec_env = doris::ExecEnv::GetInstance();
doris::ExecEnv::init(exec_env, paths);
```

# BRPC
```C++
// 2. bprc service
doris::BRpcService brpc_service(exec_env);
status = brpc_service.start(doris::config::brpc_port,
    doris::config::brpc_num_threads);
```

```C++
Status BRpcService::start(int port, int num_threads) {
    // Add service
    _server->AddService(new PInternalServiceImpl(_exec_env),
        brpc::SERVER_OWNS_SERVICE)
            .......    
}
```

# Thrift
```C++
    doris::ThriftRpcHelper::setup(exec_env);
    // 1. thrift server with be_port
    doris::ThriftServer* be_server = nullptr;
    EXIT_IF_ERROR(doris::BackendService::create_service(
        exec_env, doris::config::be_port, &be_server));
    status = be_server->start();
```
```C++
Status BackendService::create_service(
        ExecEnv* exec_env, int port, ThriftServer** server) {
    std::shared_ptr<BackendService> handler(new BackendService(exec_env));
    std::shared_ptr<ThreadFactory> thread_factory(new ThreadFactory());
    std::shared_ptr<TProcessor> be_processor(new BackendServiceProcessor(handler));
    *server = new ThriftServer("backend", be_processor, port, config::be_service_threads);
    return Status::OK();
}
```


