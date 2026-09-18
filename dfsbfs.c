#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#define N 5

// adj[u][0]、adj[u][1] 等保存顶点 u 的所有邻居。
// 用数组实现的邻接表。
int adj[N][N];

// degree[u] 表示顶点 u 当前有几个邻居。
int degree[N];

// 把数字编号转换成字母，方便输出。
char names[N] = { 'A', 'B', 'C', 'D', 'E' };

// visited[u] 为 1 表示顶点 u 已经访问过。
// visited[u] 为 0 表示顶点 u 还没有访问。
int visited[N];

// 添加一条无向边 u-v。
// 无向边要在双方邻居列表中都保存一次。
void addEdge(int u, int v)
{
    // 把 v 加入 u 的邻居列表。
    adj[u][degree[u]] = v;
    degree[u]++;

    // 把 u 加入 v 的邻居列表。
    adj[v][degree[v]] = u;
    degree[v]++;
}

// 每次开始新的遍历前，都要把 visited 全部清空。
// 否则上一次 DFS 或 BFS 的访问记录会影响这一次遍历。
void resetVisited(void)
{
    for (int i = 0; i < N; i++) {
        visited[i] = 0;
    }
}

// DFS：深度优先遍历
// 思路：访问当前点，然后递归访问第一个没访问过的邻居，
// 一条路走到底，走不通了再回来。
void dfs(int start)
{
    // 标记当前点已经访问，防止重复访问。
    visited[start] = 1;

    // 输出当前点。
    printf("%c ", names[start]);

    // 逐个查看当前点的邻居。
    for (int i = 0; i < degree[start]; i++) {
        int next = adj[start][i];

        // 如果这个邻居还没访问过，就继续往深处走。
        if (visited[next] == 0) {
            dfs(next);
        }
    }
}

// BFS：广度优先遍历
// 思路：先访问起点，再访问起点的一步邻居，
// 然后再访问两步邻居，一层一层向外扩散。
void bfs(int start)
{
    // 用一个数组模拟队列。
    // front 表示队头，rear 表示队尾。
    int queue[N];
    int front = 0;
    int rear = 0;

    // 起点入队前，先标记为已访问。
    visited[start] = 1;
    queue[rear] = start;
    rear++;

    // 只要队列里还有元素，就继续处理。
    while (front < rear) {
        // 从队头取出一个点。
        int current = queue[front];
        front++;

        // 输出当前点。
        printf("%c ", names[current]);

        // 检查当前点的所有邻居。
        for (int i = 0; i < degree[current]; i++) {
            int next = adj[current][i];

            // 如果邻居没有访问过，就加入队列。
            if (visited[next] == 0) {
                // 注意：入队时就要标记 visited，
                // 否则同一个点可能被多个邻居重复加入队列。
                visited[next] = 1;

                queue[rear] = next;
                rear++;
            }
        }
    }
}

int main(void)
{
    // 建立下面这张无向图：
    // A - B
    // A - C
    // B - D
    // B - E
    // C - E
    addEdge(0, 1);
    addEdge(0, 2);
    addEdge(1, 3);
    addEdge(1, 4);
    addEdge(2, 4);

    // 开始 DFS 前，先清空 visited。
    resetVisited();

    printf("DFS: ");
    dfs(0);
    printf("\n");

    // 开始 BFS 前，也要重新清空 visited。
    resetVisited();

    printf("BFS: ");
    bfs(0);
    printf("\n");

    return 0;
}