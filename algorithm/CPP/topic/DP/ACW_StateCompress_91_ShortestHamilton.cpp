/**
 * @brief   
 *          给定一张 n 个点的带权无向图，点从 0∼n−1 标号，
 *          求起点 0 到终点 n−1 的最短 Hamilton 路径。
 *          Hamilton 路径的定义是从 0 到 n−1 不重不漏地经过每个点恰好一次。
 *          
 *          输入格式：
 *              第一行输入整数 n。
 *              接下来 n 行每行 n 个整数，其中第 i 行第 j 个整数表示点 i 到 j 的距离（记为 a[i,j]）。
 *              对于任意的 x,y,z，数据保证 a[x,x]=0，a[x,y]=a[y,x] 并且 a[x,y]+a[y,z]≥a[x,z]。
 *          输出格式：
 *              输出一个整数，表示最短 Hamilton 路径的长度。
 *          数据范围：1≤n≤20，0≤a[i,j]≤107
 *          链接：https://www.acwing.com/problem/content/93/
 * 
 */