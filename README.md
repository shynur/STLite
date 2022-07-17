# Mini-Library-of-Template-Containers
a C++ beginner studies Data Structures by trying to write a mini library of containers<br>
**很渣, 极虚 的实现, 连代码风格都不符合我现在的审美; <br>不过确确实实地编写了数据结构整本书上几乎所有提及的内容**
____________________
简单记录一下, 只记录结构和算法方面的.<br>
像列表会记录最近一次访问的节点的位置这种实现细节就不写了 (但这个功能真的很难实现)...  

- 顺序结构 (向量, 列表, 特化的 BitMap, 实现较为残缺的 slice)  
    - 排序算法: 选择, 插入, 冒泡, 快排, 归并, 原地归并, 希儿排序, 堆排序;  
    - 串匹配:   蛮力查找, KMP;  
    - top K:   BFPRT  
- 图论 (邻接表)
    - BFS, DFS, PFS, Prim 最小生成树, Dijkstra 最短路径, bcc 双连通域分解, 拓扑排序
- 词典 (哈希表)
    - 除了 load_factor 能维持在 0.5 ~ 1 且保证动态扩缩容的均摊时间和向量一样以外, 被 std::unordered_map 完虐
    - [睿站视频演示](https://www.bilibili.com/video/BV1gZ4y1i7jH?share_source=copy_web)
- 优先级队列 (完全二叉堆)
    - Floyd 原地建堆
- [二叉搜索树另外写了](https://github.com/Shynur/Binary-Search-Tree), 它们的实现是我现在的风格, 不能和这些旧代码放在一起
- 还有一些简单的算法, 作为类方法或者函数单独存在, 比如八皇后啥的
