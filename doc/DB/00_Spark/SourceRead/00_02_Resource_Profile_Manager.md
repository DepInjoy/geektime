```plantuml
class ResourceProfileManager {
    - val resourceProfileIdToResourceProfile = new HashMap[Int, ResourceProfile]()
}

class ResourceProfile {
    - val executorResources: Map[String, ExecutorResourceRequest]
    - val taskResources: Map[String, TaskResourceRequest]
}

class TaskResourceRequest {
    - val resourceName: String
    - val amount: Double
}

class ExecutorResourceRequest{
    - val resourceName: String
    - val amount: Long
    - val discoveryScript: String
    - val vendor: String
}
ResourceProfile -up-o ResourceProfileManager
ExecutorResourceRequest -up-* ResourceProfile
TaskResourceRequest -up-* ResourceProfile
```