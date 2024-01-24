# 查看
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
# 文件拷贝
```shell
# 从容器copy文件到本地
sudo docker cp <docker_name>:<docker_abs_path> <local_path>
```
