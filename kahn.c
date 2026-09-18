#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

// 图中一共有 5 个顶点。
#define N 5

// 定义顶点编号：
// A = 0，B = 1，C = 2，D = 3，E = 4。
enum {
    A,
    B,
    C,
    D,
    E
};

// names 用来把数字编号转换成字母，方便输出。
// 例如 names[0] 是 "A"，names[1] 是 "B"。
const char* names[N] = {
    "A",
    "B",
    "C",
    "D",
    "E"
};

// 邻接表中的一个节点。
// dest 表示这条边指向哪个顶点。
// next 表示当前顶点的下一个邻居。
struct AdjNode {
    int dest;
    struct AdjNode* next;
};

// 整张图使用邻接表表示。
// head[i] 保存顶点 i 的第一个邻居。
// 后面通过 next 指针继续找到其他邻居。
struct Graph {
    struct AdjNode* head[N];
};

// 创建一张图，并初始化所有邻接表为空。
struct Graph* createGraph(void)
{
    // 申请一块内存，用来保存整张图。
    struct Graph* graph = malloc(sizeof(struct Graph));

    // 如果内存申请失败，就打印错误并退出程序。
    if (graph == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 一开始每个顶点都没有邻居，所以 head[i] 都是 NULL。
    for (int i = 0; i < N; i++) {
        graph->head[i] = NULL;
    }

    return graph;
}

// 创建一个邻接表节点。
// 例如 createNode(D) 会创建一个保存 D 的节点。
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
// 由于是有向图，这个函数只保存一个方向。
void addDirectedEdge(struct Graph* graph, int src, int dest)
{
    // 先创建一个表示 dest 的邻接节点。
    struct AdjNode* newNode = createNode(dest);

    // 如果 src 当前没有邻居，就直接把新节点作为第一个邻居。
    if (graph->head[src] == NULL) {
        graph->head[src] = newNode;
        return;
    }

    // 如果 src 已经有邻居，就沿着链表走到最后一个节点。
    struct AdjNode* current = graph->head[src];

    while (current->next != NULL) {
        current = current->next;
    }

    // 把新节点接到链表末尾。
    current->next = newNode;
}

// Kahn 算法：对有向图进行拓扑排序。
// 如果图中存在环，就无法完成拓扑排序。
void topologicalSort(struct Graph* graph)
{
    // indegree[i] 表示顶点 i 的入度。
    // 入度就是有多少条边指向顶点 i。
    // 这里把所有入度初始化为 0。
    int indegree[N] = { 0 };

    // queue 是模拟队列的数组。
    // 入度为 0 的顶点会先进入队列。
    int queue[N];

    // order 用来保存最终的拓扑排序结果。
    int order[N];

    // front 表示队头位置。
    // rear 表示队尾后面第一个空位置。
    int front = 0;
    int rear = 0;

    // count 记录已经加入拓扑排序结果的顶点数量。
    int count = 0;

    // 第一步：遍历所有有向边，计算每个顶点的入度。
    for (int node = 0; node < N; node++) {
        // 从当前顶点的第一个邻居开始。
        struct AdjNode* current = graph->head[node];

        // 遍历当前顶点的所有出边。
        while (current != NULL) {
            // node -> current->dest
            // 所以 current->dest 的入度加 1。
            indegree[current->dest]++;

            current = current->next;
        }
    }

    // 第二步：把所有入度为 0 的顶点加入队列。
    // 入度为 0 表示这个顶点没有前置依赖，可以立即处理。
    for (int node = 0; node < N; node++) {
        if (indegree[node] == 0) {
            queue[rear] = node;

            // 加入一个元素后，队尾向后移动。
            rear++;
        }
    }

    // 第三步：不断从队列中取出顶点，并删除它的出边。
    while (front < rear) {
        // 取出队头顶点。
        int node = queue[front];

        // 队头向后移动。
        front++;

        // 把这个顶点加入拓扑排序结果。
        order[count] = node;
        count++;

        // 遍历当前顶点的所有出边。
        struct AdjNode* current = graph->head[node];

        while (current != NULL) {
            // 找到当前边指向的邻居。
            int neighbor = current->dest;

            // 把当前边 node -> neighbor 删除。
            // 所以 neighbor 的入度减少 1。
            indegree[neighbor]--;

            // 如果 neighbor 的入度变成 0，
            // 说明它的所有前置顶点都已经处理完了。
            if (indegree[neighbor] == 0) {
                // 把 neighbor 加入队列，等待后续处理。
                queue[rear] = neighbor;

                // 队尾向后移动。
                rear++;
            }

            // 继续处理当前顶点的下一条出边。
            current = current->next;
        }
    }

    // 如果成功排序的顶点数量不等于总顶点数，
    // 说明图中存在环。
    //
    // 因为环上的顶点始终存在入边，
    // 它们的入度不会变成 0，所以无法进入队列。
    if (count != N) {
        printf("Cycle detected: no topological order.\n");
        return;
    }

    // 输出拓扑排序结果。
    printf("Topological order: ");

    for (int i = 0; i < count; i++) {
        // 除了第一个顶点，后面每个顶点前面都打印箭头。
        if (i > 0) {
            printf(" -> ");
        }

        // 把数字编号转换成字母输出。
        printf("%s", names[order[i]]);
    }

    printf("\n");
}

// 释放图占用的内存。
// 先释放每条邻接链表中的节点，再释放整张图。
void freeGraph(struct Graph* graph)
{
    for (int i = 0; i < N; i++) {
        // 从顶点 i 的第一个邻居开始。
        struct AdjNode* current = graph->head[i];

        // 逐个释放邻接链表节点。
        while (current != NULL) {
            struct AdjNode* next = current->next;

            // 释放当前节点。
            free(current);

            // 继续处理下一个节点。
            current = next;
        }
    }

    // 最后释放整张图。
    free(graph);
}

int main(void)
{
    // 创建一张 DAG，也就是有向无环图。
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

    printf("DAG:\n");

    // 对 DAG 进行拓扑排序。
    topologicalSort(dag);

    // 再创建一张有环图：
    //
    // A -> B
    // B -> C
    // C -> A
    //
    // 这张图无法进行拓扑排序。
    struct Graph* cycleGraph = createGraph();

    addDirectedEdge(cycleGraph, A, B);
    addDirectedEdge(cycleGraph, B, C);
    addDirectedEdge(cycleGraph, C, A);

    printf("Graph with a cycle:\n");

    // 对有环图进行拓扑排序，程序应该检测到环。
    topologicalSort(cycleGraph);

    // 释放两张图占用的内存。
    freeGraph(dag);
    freeGraph(cycleGraph);

    return 0;
}