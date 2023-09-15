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
