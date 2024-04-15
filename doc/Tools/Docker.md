# Docker命令
## 镜像操作
```shell
# 列出所有镜像来获取镜像的ID，镜像的名称和其他详细信息
docker images

# 删除镜像
docker rmi <your-image-id>
```

## 查看
列出容器的命令是`docker container ls`,等价的命令有
```shell
docker ps
docker container ps
docker container list
```
常用参数
```
-a, --all：     列出所有的容器，包括停止运行的容器
-s, --size：    显示容器的大小
-q, --quiet：   仅显示容器ID
-f, --filter：  过滤器，支持key=value的格式进行过滤
                多个过滤器使用-f "key=value" -f "key=value"格式
```
## 文件拷贝
```shell
# 从容器copy文件到本地
sudo docker cp <docker_name>:<docker_abs_path> <local_path>
```

根据docker name删除docker
```shell
docker remove <docker-name>
```

## 启动容器
```shell
# 以bash交互方式启动容器
docker run -it <docker-name> /bin/bash
```

# 应用场景

## 重新进入Exited容器

```shell
# 确认容器是否还存在
docker ps -a | grep <container name/id>

# 重新启动容器
docker start -ia <container name/id>

# 以bash交互方式进入容器
docker exec -it <container name/id> /bin/bash
```
