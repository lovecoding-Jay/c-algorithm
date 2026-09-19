#include <stdio.h>
#include <stdlib.h>

// 图中一共有 5 个顶点。
#define N 5

// 图中有 6 条带权边。
#define EDGE_COUNT 6

// 给 5 个顶点定义编号：
// A = 0，B = 1，C = 2，D = 3，E = 4。
enum {
    A,
    B,
    C,
    D,
    E
};

// names 用来把数字编号转换成字母。
//  names[0] 是 "A"，names[1] 是 "B"。
const char* names[N] = {
    "A",
    "B",
    "C",
    "D",
    "E"
};

// 邻接表中的一个节点。
// dest 表示当前边指向哪个顶点。
// next 表示当前顶点的下一个邻居。
struct AdjNode {
    int dest;
    struct AdjNode* next;
};

// 图使用邻接表表示。
// head[i] 保存顶点 i 的第一个邻居。
// 其他邻居通过链表 next 继续连接。
struct Graph {
    struct AdjNode* head[N];
};

// visited 用于 DFS 和 BFS。
// visited[i] = 1 表示顶点 i 已经访问过。
// visited[i] = 0 表示顶点 i 还没有访问。
int visited[N];

// 带权边的结构体。
// u 和 v 是边的两个端点。
// weight 是边的权重。
struct Edge {
    int u;
    int v;
    int weight;
};

// 并查集中的 parent 数组。
// parent[x] 表示 x 所属集合的父节点。
int parent[N];

// rankValue 用来记录集合树的高度。
// 合并时把矮的树挂到高的树下面，可以避免并查集退化成很长的链。
int rankValue[N];

// 创建一张空图。
struct Graph* createGraph(void)
{
    // 申请一块内存，用来保存整张图。
    struct Graph* graph = malloc(sizeof(struct Graph));

    // 如果内存申请失败，就打印错误并退出程序。
    if (graph == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 一开始每个顶点都没有邻居。
    for (int i = 0; i < N; i++) {
        graph->head[i] = NULL;
    }

    return graph;
}

// 创建一个邻接表节点。
// 如 createNode(B) 会创建一个保存 B 的节点。
struct AdjNode* createNode(int dest)
{
    // 申请一个邻接表节点的内存。
    struct AdjNode* newNode = malloc(sizeof(struct AdjNode));

    // 检查内存是否申请成功。
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 保存目标顶点编号。
    newNode->dest = dest;

    // 新节点后面暂时没有其他邻居。
    newNode->next = NULL;

    return newNode;
}

// 添加一条有向边：src -> dest。
// 因为这个函数只添加一个方向，
// 所以它表示的是有向图。
void addDirectedEdge(struct Graph* graph, int src, int dest)
{
    // 创建一个表示 dest 的邻接节点。
    struct AdjNode* newNode = createNode(dest);

    // 如果 src 当前没有邻居，
    // 就把新节点直接作为第一个邻居。
    if (graph->head[src] == NULL) {
        graph->head[src] = newNode;
        return;
    }

    // 如果 src 已经有邻居，
    // 就沿着链表走到最后一个节点。
    struct AdjNode* current = graph->head[src];

    while (current->next != NULL) {
        current = current->next;
    }

    // 把新节点接到链表末尾。
    current->next = newNode;
}

// 添加一条无向边：first - second。
// 无向边表示两个方向都能走，
// 所以要同时添加 first -> second 和 second -> first。
void addUndirectedEdge(struct Graph* graph, int first, int second)
{
    addDirectedEdge(graph, first, second);
    addDirectedEdge(graph, second, first);
}

// 清空 visited 数组。
// 每次重新开始 DFS 或 BFS 前都要调用。
void clearVisited(void)
{
    for (int i = 0; i < N; i++) {
        visited[i] = 0;
    }
}

// DFS：深度优先遍历。
// 思路：访问当前点，然后一直向第一个未访问的邻居深入。
// 走到不能再走时，再回到上一个点继续找其他邻居。
void dfs(struct Graph* graph, int start)
{
    // 先把当前点标记为已访问，防止重复访问。
    visited[start] = 1;

    // 输出当前点。
    printf("%s", names[start]);

    // 找到当前点的第一个邻居。
    struct AdjNode* current = graph->head[start];

    // 依次检查所有邻居。
    while (current != NULL) {
        // 如果邻居没有访问过，就继续递归访问。
        if (!visited[current->dest]) {
            // 先输出箭头，再进入下一层递归。
            printf(" -> ");

            dfs(graph, current->dest);
        }

        // 处理下一个邻居。
        current = current->next;
    }
}

// BFS：广度优先遍历。
// 思路：先访问起点的一层邻居，
// 再访问下一层邻居，一层一层向外扩散。
void bfs(struct Graph* graph, int start)
{
    // 使用数组模拟队列。
    // front 表示队头，rear 表示队尾。
    int queue[N];
    int front = 0;
    int rear = 0;

    // first 用来控制箭头输出。
    // 第一个顶点前面不需要箭头。
    int first = 1;

    // BFS 开始前清空访问记录。
    clearVisited();

    // 起点入队前标记为已访问。
    visited[start] = 1;

    // 起点入队。
    queue[rear] = start;
    rear++;

    // 只要队列里还有元素，就继续处理。
    while (front < rear) {
        // 从队头取出一个顶点。
        int node = queue[front];
        front++;

        // 如果不是第一个输出点，就先输出箭头。
        if (!first) {
            printf(" -> ");
        }

        // 输出当前顶点。
        printf("%s", names[node]);
        first = 0;

        // 找到当前顶点的第一个邻居。
        struct AdjNode* current = graph->head[node];

        // 依次检查所有邻居。
        while (current != NULL) {
            // 如果邻居没有访问过，就加入队列。
            if (!visited[current->dest]) {
                // 注意：入队时就要标记 visited。
                // 否则同一个点可能被多个邻居重复加入队列。
                visited[current->dest] = 1;

                queue[rear] = current->dest;
                rear++;
            }

            current = current->next;
        }
    }
}

// Kahn 算法：拓扑排序。
// 拓扑排序要求每一条边 u -> v 中，
// u 都必须排在 v 前面。
void topologicalSort(struct Graph* graph)
{
    // indegree[i] 表示顶点 i 的入度。
    // 入度就是有多少条边指向顶点 i。
    int indegree[N] = { 0 };

    // queue 用来保存当前入度为 0 的顶点。
    int queue[N];

    // order 用来保存拓扑排序结果。
    int order[N];

    // front 表示队头，rear 表示队尾。
    int front = 0;
    int rear = 0;

    // count 表示已经成功加入拓扑序的顶点数量。
    int count = 0;

    // 第一步：计算每个顶点的入度。
    for (int node = 0; node < N; node++) {
        // 找到当前顶点的第一个邻居。
        struct AdjNode* current = graph->head[node];

        // 遍历所有出边。
        while (current != NULL) {
            // node -> current->dest
            // 所以 current->dest 的入度加 1。
            indegree[current->dest]++;

            current = current->next;
        }
    }

    // 第二步：把入度为 0 的顶点加入队列。
    // 入度为 0 表示它没有前置依赖，可以立即处理。
    for (int node = 0; node < N; node++) {
        if (indegree[node] == 0) {
            queue[rear] = node;
            rear++;
        }
    }

    // 第三步：不断从队列取出顶点。
    while (front < rear) {
        // 取出队头顶点。
        int node = queue[front];
        front++;

        // 把当前顶点加入拓扑排序结果。
        order[count] = node;
        count++;

        // 找到当前顶点的第一个邻居。
        struct AdjNode* current = graph->head[node];

        // 删除当前顶点的所有出边。
        while (current != NULL) {
            int neighbor = current->dest;

            // 删除当前边后，邻居入度减 1。
            indegree[neighbor]--;

            // 如果邻居的入度变成 0，
            // 说明它的所有前置顶点都已经处理完了。
            if (indegree[neighbor] == 0) {
                queue[rear] = neighbor;
                rear++;
            }

            current = current->next;
        }
    }

    // 如果成功排序的顶点数量不等于总顶点数，
    // 说明图中存在环。
    //
    // 因为环上的顶点始终有入边，
    // 它们的入度永远不会变成 0。
    if (count != N) {
        printf("No topological order (cycle detected).\n");
        return;
    }

    // 输出拓扑排序结果。
    for (int i = 0; i < count; i++) {
        if (i > 0) {
            printf(" -> ");
        }

        printf("%s", names[order[i]]);
    }

    printf("\n");
}

// 初始化并查集。
// 一开始每个顶点都属于自己单独的集合。
void initializeDSU(int nodeCount)
{
    for (int i = 0; i < nodeCount; i++) {
        // 每个顶点一开始都是自己的根节点。
        parent[i] = i;

        // 每个集合一开始高度为 0。
        rankValue[i] = 0;
    }
}

// 查找某个顶点所在集合的根节点。
// 如果两个顶点找到的根相同，
// 说明它们已经连通。
int findRoot(int node)
{
    // 如果 parent[node] 不等于 node，
    // 说明 node 还有父节点，需要继续向上查找。
    while (parent[node] != node) {
        // 路径压缩：
        // 让当前节点直接指向爷爷节点，
        // 减少以后查找根节点经过的层数。
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
    int rootFirst = findRoot(first);
    int rootSecond = findRoot(second);

    // 根相同，说明两个顶点已经连通。
    // 此时加入边会形成环。
    if (rootFirst == rootSecond) {
        return 0;
    }

    // 按秩合并：
    // 把高度较小的集合挂到高度较大的集合下面。
    if (rankValue[rootFirst] < rankValue[rootSecond]) {
        int temp = rootFirst;
        rootFirst = rootSecond;
        rootSecond = temp;
    }

    // 把 rootSecond 集合挂到 rootFirst 集合下面。
    parent[rootSecond] = rootFirst;

    // 如果两个集合高度相同，
    // 合并后高度会增加 1。
    if (rankValue[rootFirst] == rankValue[rootSecond]) {
        rankValue[rootFirst]++;
    }

    // 返回 1，表示成功合并。
    return 1;
}

// qsort 比较函数。
// 返回值小于 0：第一条边排在前面。
// 返回值大于 0：第二条边排在前面。
// 返回值等于 0：两条边权重相同。
int compareEdges(const void* first, const void* second)
{
    const struct Edge* edgeFirst = (const struct Edge*)first;
    const struct Edge* edgeSecond = (const struct Edge*)second;

    // 权重小的边排在前面。
    return edgeFirst->weight - edgeSecond->weight;
}

// Kruskal 最小生成树算法。
void kruskal(struct Edge edges[], int edgeCount, int nodeCount)
{
    // 初始化并查集。
    initializeDSU(nodeCount);

    // 把边按照权重从小到大排序。
    qsort(edges, edgeCount, sizeof(struct Edge), compareEdges);

    // selectedCount 记录已经选了多少条边。
    int selectedCount = 0;

    // totalWeight 记录最小生成树的总权重。
    int totalWeight = 0;

    // mst 用来保存最终选出的边。
    struct Edge mst[N];

    printf("Kruskal process:\n");

    // 依次检查每一条边。
    for (int i = 0; i < edgeCount; i++) {
        int u = edges[i].u;
        int v = edges[i].v;
        int weight = edges[i].weight;

        printf(
            "Checking %s - %s (%d): ",
            names[u],
            names[v],
            weight
        );

        // 判断 u 和 v 是否已经连通。
        // 如果返回 1，说明可以加入这条边。
        // 如果返回 0，说明会形成环，需要跳过。
        if (unionSets(u, v)) {
            printf("selected\n");

            // 保存这条边。
            mst[selectedCount] = edges[i];

            // 已选边数量加 1。
            selectedCount++;

            // 累加权重。
            totalWeight += weight;

            // 生成树有 n-1 条边。
            // 选够以后就可以停止。
            if (selectedCount == nodeCount - 1) {
                printf("Reached n - 1 edges, stop.\n");
                break;
            }
        }
        else {
            printf("skipped, would create a cycle\n");
        }
    }

    // 如果最后选不够 n-1 条边，
    // 说明图不连通，不存在生成树。
    if (selectedCount != nodeCount - 1) {
        printf("The graph is disconnected.\n");
        printf("No spanning tree exists.\n");
        return;
    }

    // 输出最终选出的最小生成树。
    printf("\nMinimum spanning tree:\n");

    for (int i = 0; i < selectedCount; i++) {
        printf(
            "%s - %s : %d\n",
            names[mst[i].u],
            names[mst[i].v],
            mst[i].weight
        );
    }

    // 输出总权重。
    printf("Total weight: %d\n", totalWeight);
}

// 释放图占用的内存。
void freeGraph(struct Graph* graph)
{
    for (int i = 0; i < N; i++) {
        // 从顶点 i 的第一个邻居开始。
        struct AdjNode* current = graph->head[i];

        // 逐个释放邻接链表中的节点。
        while (current != NULL) {
            struct AdjNode* next = current->next;

            free(current);

            current = next;
        }
    }

    // 最后释放图本身。
    free(graph);
}

int main(void)
{
    // 建立无向图，测试 DFS、BFS 和 Kruskal。
    //
    // A - B
    // A - C
    // B - D
    // B - E
    // C - E
    struct Graph* graph = createGraph();

    addUndirectedEdge(graph, A, B);
    addUndirectedEdge(graph, A, C);
    addUndirectedEdge(graph, B, D);
    addUndirectedEdge(graph, B, E);
    addUndirectedEdge(graph, C, E);

    // 清空访问记录后执行 DFS。
    clearVisited();
    printf("DFS: ");
    dfs(graph, A);
    printf("\n");

    // 清空访问记录后执行 BFS。
    clearVisited();
    printf("BFS: ");
    bfs(graph, A);
    printf("\n");

    // 建立 DAG，用于测试拓扑排序。
    //
    // A -> B
    // A -> C
    // B -> D
    // C -> D
    // D -> E
    struct Graph* dag = createGraph();

    addDirectedEdge(dag, A, B);
    addDirectedEdge(dag, A, C);
    addDirectedEdge(dag, B, D);
    addDirectedEdge(dag, C, D);
    addDirectedEdge(dag, D, E);

    printf("Topological order: ");
    topologicalSort(dag);

    // 建立有环图，用于测试环检测。
    //
    // A -> B
    // B -> C
    // C -> A
    struct Graph* cyclicGraph = createGraph();

    addDirectedEdge(cyclicGraph, A, B);
    addDirectedEdge(cyclicGraph, B, C);
    addDirectedEdge(cyclicGraph, C, A);

    printf("Cyclic graph: ");
    topologicalSort(cyclicGraph);

    // Kruskal 使用带权边表。
    // 无向边只需要保存一次。
    struct Edge edges[EDGE_COUNT] = {
        {A, B, 1},
        {B, C, 2},
        {A, C, 3},
        {C, D, 4},
        {D, E, 5},
        {B, D, 6}
    };

    kruskal(edges, EDGE_COUNT, N);

    // 释放动态申请的内存。
    freeGraph(graph);
    freeGraph(dag);
    freeGraph(cyclicGraph);

    return 0;
}