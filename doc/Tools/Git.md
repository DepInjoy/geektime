# Git Tag

```shell
# 创建一个带备注的Tag
git tag -a V20210311 -m 'Update friend application message'

# 将Tag推送到远程
git push origin --tags
```

```shell
# 查看当前项目有哪些tag
git tag

# 查看Tag对应的版本号,这里可以看到commit版本号
git show tagname
```

```shell
# 回退到相应的版本
git reset --hard <commit版本号>
```

```shell
# 删除本地tag
git tag -d tagname

# 删除远程tag
git push origin :refs/tags/tagname
git push origin --delete tag tagname
```

[git tag的查看、添加、推送和删除](http://www.git.kim/gitjishu/69.html)

# submodle(子模块)

```shell
# 自动初始化并更新仓库中的每一个子模块， 包括可能存在的嵌套子模块
git clone --recurse-submodules <git_url>
```

在clone有子模块的项目时，没有更新子模块，需要init和update来更新子模块
```shell
# 初始化本地配置文件
git submodule init
# 从该项目中抓取所有数据并检出父项目中列出的合适的提交
git submodule update

# 将上面的两步合并为一步
# 实现初始化、抓取并检出任何嵌套的子模块
git submodule update --init --recursive
```

## 参考资料
1. [Git工具 - 子模块](https://git-scm.com/book/zh/v2/Git-%E5%B7%A5%E5%85%B7-%E5%AD%90%E6%A8%A1%E5%9D%97)

# Git报错处理

## RPC failed; curl 56 GnuTLS recv error (-54)
git clone 代码执行多次都提示下面的错误信息
```
remote: Enumerating objects: 278590, done.
remote: Counting objects: 100% (1723/1723), done.
remote: Compressing objects: 100% (1002/1002), done.
error: RPC failed; curl 56 GnuTLS recv error (-54): Error in the pull function.
fatal: The remote end hung up unexpectedly
fatal: early EOF
fatal: index-pack failed
```

在StackOverflow[1]中找到如下的解决方法：
```shell
git clone http://github.com/large-repository --depth 1
cd large-repository
git fetch --unshallow
```

## RPC failed; result=18, HTTP code = 2008 MiB | 2.73 MiB/s
在`git submodule update`多次报上述错误
```
git config http.postBuffer 24288000
```
在StackOverflow[2]中找到如下的解决方法：
```
```

## 参考资料
1. [StackOverflow:RPC failed; curl transfer closed with outstanding read data remaining](https://stackoverflow.com/questions/38618885/error-rpc-failed-curl-transfer-closed-with-outstanding-read-data-remaining)
2. [git bash: error: RPC failed; result = 18, HTP code = 200B | 1KiB/s](https://stackoverflow.com/questions/17683295/git-bash-error-rpc-failed-result-18-htp-code-200b-1kib-s)