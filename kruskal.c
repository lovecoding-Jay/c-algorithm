#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

// 图中一共有 5 个顶点。
#define N 5

// 一共有 6 条边。
#define EDGE_COUNT 6

// 顶点编号：
// A = 0，B = 1，C = 2，D = 3，E = 4。
enum {
    A,
    B,
    C,
    D,
    E
};

// names 用来把数字编号转换成字母，方便输出。
const char* names[N] = {
    "A",
    "B",
    "C",
    "D",
    "E"
};

// 一条带权重的边。
// u 和 v 是边的两个端点，weight 是边的权重。
struct Edge {
    int u;
    int v;
    int weight;
};

// parent 是并查集的父节点数组。
// parent[x] 表示 x 所在集合的代表元素。
int parent[N];

// rankValue 用来记录集合树的高度。
// 它的作用是让并查集尽量保持平衡，避免变成很长的链。
int rankValue[N];

// 初始化并查集。
// 一开始每个顶点都属于自己单独的集合。
void initializeDSU(int nodeCount)
{
    for (int i = 0; i < nodeCount; i++) {
        // 每个顶点一开始都是自己的根节点。
        parent[i] = i;

        // 每个集合一开始高度都是 0。
        rankValue[i] = 0;
    }
}

// 查找某个顶点所在集合的根节点。
// 如果两个顶点找到的根节点相同，说明它们已经连通。
int findRoot(int node)
{
    // 如果 parent[node] 不是 node，
    // 说明 node 还有父节点，需要继续向上查找。
    while (parent[node] != node) {
        // 路径压缩：
        // 让当前节点直接指向它的爷爷节点，
        // 减少以后查找根节点需要经过的层数。
        parent[node] = parent[parent[node]];

        // 继续向根节点移动。
        node = parent[node];
    }

    return node;
}

// 合并两个顶点所在的集合。
//
// 返回值：
// 返回 1：两个集合原来不连通，合并成功。
// 返回 0：两个顶点已经连通，不能再加入这条边。
int unionSets(int first, int second)
{
    // 找到两个顶点所在集合的根节点。
    int rootFirst = findRoot(first);
    int rootSecond = findRoot(second);

    // 如果两个根相同，说明两个顶点已经连通。
    // 这时再加入边就会形成环。
    if (rootFirst == rootSecond) {
        return 0;
    }

    // 按秩合并：
    // 让高度较小的集合挂到高度较大的集合下面。
    if (rankValue[rootFirst] < rankValue[rootSecond]) {
        int temp = rootFirst;
        rootFirst = rootSecond;
        rootSecond = temp;
    }

    // 把 rootSecond 这个集合挂到 rootFirst 下面。
    parent[rootSecond] = rootFirst;

    // 如果两个集合高度相同，合并后高度会增加 1。
    if (rankValue[rootFirst] == rankValue[rootSecond]) {
        rankValue[rootFirst]++;
    }

    // 返回 1，表示两个集合成功合并。
    return 1;
}

// qsort 的比较函数。
// qsort 会根据这个函数的返回值决定边的先后顺序。
int compareEdges(const void* first, const void* second)
{
    // qsort 传进来的是 void 指针，
    // 所以要先转换成 struct Edge 指针。
    const struct Edge* edgeFirst = (const struct Edge*)first;
    const struct Edge* edgeSecond = (const struct Edge*)second;

    // 返回负数：第一条边排在前面。
    // 返回正数：第二条边排在前面。
    // 返回 0：两条边权重相同。
    return edgeFirst->weight - edgeSecond->weight;
}

// Kruskal 算法。
// edges 是边表，edgeCount 是边的数量，nodeCount 是顶点数量。
void kruskal(struct Edge edges[], int edgeCount, int nodeCount)
{
    // 初始化并查集。
    initializeDSU(nodeCount);

    // 第一步：把所有边按照权重从小到大排序。
    // 这样每次都能先尝试权重最小的边。
    qsort(edges, edgeCount, sizeof(struct Edge), compareEdges);

    // selectedCount 记录已经选了多少条边。
    int selectedCount = 0;

    // totalWeight 记录最小生成树的总权重。
    int totalWeight = 0;

    // 用来保存最终选出的边。
    struct Edge mst[N];

    printf("Kruskal process:\n");

    // 第二步：依次检查每一条边。
    for (int i = 0; i < edgeCount; i++) {
        // 取出当前边的两个端点和权重。
        int u = edges[i].u;
        int v = edges[i].v;
        int weight = edges[i].weight;

        printf(
            "Checking %s - %s (%d): ",
            names[u],
            names[v],
            weight
        );

        // 第三步：判断 u 和 v 是否已经连通。
        //
        // 如果 unionSets 返回 1：
        //     说明 u 和 v 不在同一个集合中，
        //     可以加入这条边，并把两个集合合并。
        //
        // 如果 unionSets 返回 0：
        //     说明 u 和 v 已经连通，
        //     加入这条边会形成环，所以跳过。
        if (unionSets(u, v)) {
            printf("selected\n");

            // 把这条边保存到最小生成树中。
            mst[selectedCount] = edges[i];

            // 已选边数量增加。
            selectedCount++;

            // 累加这条边的权重。
            totalWeight += weight;

            // 最小生成树一共有 nodeCount - 1 条边。
            // 选够之后就停止。
            if (selectedCount == nodeCount - 1) {
                printf("Reached n - 1 edges, stop.\n");
                break;
            }
        }
        else {
            // 两个端点已经连通，选择这条边会形成环。
            printf("skipped, would create a cycle\n");
        }
    }

    // 如果遍历完所有边之后，选出的边数还是不够，
    // 说明图不连通，不存在生成树。
    if (selectedCount != nodeCount - 1) {
        printf("The graph is disconnected.\n");
        printf("No spanning tree exists.\n");
        return;
    }

    // 输出最终选择出来的最小生成树。
    printf("\nMinimum spanning tree:\n");

    for (int i = 0; i < selectedCount; i++) {
        int u = mst[i].u;
        int v = mst[i].v;
        int weight = mst[i].weight;

        printf(
            "%s - %s : %d\n",
            names[u],
            names[v],
            weight
        );
    }

    // 输出最小生成树的总权重。
    printf("Total weight: %d\n", totalWeight);
}

int main(void)
{
    // 建立一张无向带权图：
    //
    // A - B : 1
    // B - C : 2
    // A - C : 3
    // C - D : 4
    // D - E : 5
    // B - D : 6
    //
    // 注意：无向边只需要保存一次。
    // 不要同时写 A-B 和 B-A。
    struct Edge edges[EDGE_COUNT] = {
        {A, B, 1},
        {B, C, 2},
        {A, C, 3},
        {C, D, 4},
        {D, E, 5},
        {B, D, 6}
    };

    // 调用 Kruskal 算法。
    kruskal(edges, EDGE_COUNT, N);

    return 0;
}