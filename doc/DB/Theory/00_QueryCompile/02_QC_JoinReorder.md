# 贪心启发算法
当需要Join的表数量过多(通常超过10个)适合贪心，其优势在于可以较快第找到还不错的Join Order。其核心思想是，从一张表扩展到N张表，每次选出当前代价最小的一张表，加入Join Tree，构建出Left Deep Tree。贪心算法也有很多扩展，主要扩展点围绕避免局部最优以及产生Bushy Tree。

# 枚举算法
主流的两种
- 基于规则变换的Top-down枚举，可以结合Top-down cascasde 框架通过记忆化的方式来实现。
- 基于DP的bottom-up 枚举，典型代表DPhyp算法，其优势在于可以高效的产生bushy tree

一般情况下，数据库系统会把贪心和枚举有效的结合，从而对任意数量的表join 都能够在合理的时间内得到有效的Join Order。

