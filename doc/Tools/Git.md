
```shell
# 更新Git远程分支列表
git remote update origin --prune
```
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

# 分支管理
```shell
# 拉取远程仓库的最新变化，只更新本地的远程追踪分支
git fetch origin

# 拉取并合并远程分支的内容到当前分支
git pull origin <远程分支名>
```

# 日常查询定位
## 查看某个文件某行的改动历史
使用`git blame`和`git show`命令，查看一个文件，某些行的修改记录。相关步骤:
1. 获取修改那几行代码的commitID;
2. 根据commitID查看那个文件的该次改动的详细情况。

```shell
git blame -L m,n <file_path>
git show commitID
```
### 参考资料
1. [git查看某个文件某些行的改动](https://blog.csdn.net/zhezhebie/article/details/82378809)

## 合并某分支并删除某提交
```shell
# 首先切换到目标分支
git checkout <目标分支名>

# 合并指定分支，用--no-commit暂时不提交合并结果
git merge --no-commit <要合并的分支名>

# 剔除不想要的提交，可以通过git log查看提交的哈希值
git revert <不想要的提交的哈希值>
```
如果剔除不想要的提交，存在冲突，手动合并冲突，执行`git revert --continue`

最后，提交合并结果
```shell
git commit -m "xxx"
```

如果想完全删除某次提交的痕迹，可以考虑使用`git rebase -i`来修改提交历史

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