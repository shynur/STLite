# Simple Implementation of Template Container Library

很渣, 极虚 的个人向实现.
内容参考 *邓俊辉-数据结构*, 重写了书中所有的容器代码.

____________________

简单记录一下, 只结构和算法方面.[^1]  <br />

- 顺序结构: 向量, 列表, 特化的 BitMap, 对 Rust slice 的拙劣模仿 (特别是 auto deref)
  - 排序算法: 选择, 插入, 冒泡, 快排, 归并, 原地归并, 希儿排序, 堆排序
  - 串匹配: 蛮力查找, KMP, BM
  - top K: BFPRT
- 图论: 邻接表
  - BFS, DFS, PFS
  - Prim 最小生成树
  - Dijkstra 最短路径
  - bcc 双连通域分解
  - 拓扑排序
- 词典: 哈希表, [平衡二叉搜索树](https://github.com/shynur/Binary-Search-Tree) (另见)
  - 闭散列
  - 平方试探法
  - 除了 load factor 能维持在 0.5-1 且保证动态扩缩容的均摊时间和向量一样以外, 被 `std::unordered_map` 完虐.  [睿站视频演示](https://www.bilibili.com/video/BV1gZ4y1i7jH)
- 优先级队列: 完全二叉堆
    - Floyd 原地建堆

[^1]: 像列表会记录最近一次访问的节点的位置这种实现细节就不写了.
