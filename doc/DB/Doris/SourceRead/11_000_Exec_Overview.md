# 前端和后端对接

前端`Coordinator::exec`接口发起RPC将计划发送给后端，相关接口
```java
public void exec() throws Exception {
		....
        if (enablePipelineEngine) {
            sendPipelineCtx();
        } else {
            sendFragment();
        }
        ......
}
```

后端调用执行计算的入口
```C++
void PInternalServiceImpl::exec_plan_fragment(
        google::protobuf::RpcController* controller,
        const PExecPlanFragmentRequest* request,
        PExecPlanFragmentResult* response,
        google::protobuf::Closure* done) {
    bool ret = _light_work_pool.try_offer([this, controller, request, response, done]() {
        _exec_plan_fragment_in_pthread(controller, request, response, done);
    });
    if (!ret) {
        offer_failed(response, done, _light_work_pool);
        return;
    }
}


void PInternalServiceImpl::_exec_plan_fragment_in_pthread(
        google::protobuf::RpcController* controller, 
        const PExecPlanFragmentRequest* request,
        PExecPlanFragmentResult* response, google::protobuf::Closure* done) {
    brpc::ClosureGuard closure_guard(done);
    auto st = Status::OK();
    bool compact = request->has_compact() ? request->compact() : false;
    PFragmentRequestVersion version = request->has_version() ?
        request->version() : PFragmentRequestVersion::VERSION_1;
                ......
    // 实际执行
    st = _exec_plan_fragment_impl(request->request(), version, compact);
    st.to_protobuf(response->mutable_status());
}

Status PInternalServiceImpl::_exec_plan_fragment_impl(
    const std::string& ser_request,
    PFragmentRequestVersion version, bool compact) {
    if (version == PFragmentRequestVersion::VERSION_1) {
        // VERSION_1 should be removed in v1.2
        TExecPlanFragmentParams t_request;
        return _exec_env->fragment_mgr()->exec_plan_fragment(t_request);
    } else if (version == PFragmentRequestVersion::VERSION_2) {
        // 火山模型
        TExecPlanFragmentParamsList t_request;
        const auto& fragment_list = t_request.paramsList;
                ......
        for (const TExecPlanFragmentParams& params : t_request.paramsList) {
            RETURN_IF_ERROR(_exec_env->fragment_mgr()->exec_plan_fragment(params));
        }
    } else if (version == PFragmentRequestVersion::VERSION_3) {
        // Pipeline
        TPipelineFragmentParamsList t_request;
                ......
        const auto& fragment_list = t_request.params_list;
        for (const TPipelineFragmentParams& fragment : fragment_list) {
            RETURN_IF_ERROR(_exec_env->fragment_mgr()->exec_plan_fragment(fragment));
        }
                ......
    } else {
        return Status::InternalError("invalid version");
    }
    return Status::OK();
}
```