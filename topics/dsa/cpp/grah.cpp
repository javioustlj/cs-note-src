#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>
#include <algorithm>

using namespace std;

// 图类定义
class Graph {
private:
    int V; // 顶点数
    vector<vector<int>> adj; // 邻接表
    bool directed; // 是否为有向图

public:
    // 构造函数
    Graph(int vertices, bool isDirected = false) : V(vertices), directed(isDirected) {
        adj.resize(V);
    }

    // 添加边
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        if (!directed)
            adj[v].push_back(u); // 无向图需要添加双向边
    }

    // 有向图环检测 - 使用DFS
    bool hasCycleDirected() {
        vector<bool> visited(V, false);
        vector<bool> recStack(V, false);

        for (int i = 0; i < V; ++i)
            if (!visited[i] && dfsCycleDirected(i, visited, recStack))
                return true;

        return false;
    }

    // 有向图DFS辅助函数
    bool dfsCycleDirected(int v, vector<bool>& visited, vector<bool>& recStack) {
        visited[v] = true;
        recStack[v] = true;

        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                if (dfsCycleDirected(neighbor, visited, recStack))
                    return true;
            } else if (recStack[neighbor]) {
                return true;
            }
        }

        recStack[v] = false;
        return false;
    }

    // 无向图环检测 - 使用DFS
    bool hasCycleUndirected() {
        if (directed)
            throw invalid_argument("该方法仅适用于无向图");

        vector<bool> visited(V, false);

        for (int i = 0; i < V; ++i)
            if (!visited[i] && dfsCycleUndirected(i, visited, -1))
                return true;

        return false;
    }

    // 无向图DFS辅助函数
    bool dfsCycleUndirected(int v, vector<bool>& visited, int parent) {
        visited[v] = true;

        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                if (dfsCycleUndirected(neighbor, visited, v))
                    return true;
            } else if (neighbor != parent) {
                return true;
            }
        }

        return false;
    }

    // 无向图环检测 - 使用并查集(Union-Find)
    bool hasCycleUndirectedUnionFind() {
        if (directed)
            throw invalid_argument("该方法仅适用于无向图");

        vector<int> parent(V, -1);

        // 遍历所有边，检查是否形成环
        for (int u = 0; u < V; ++u) {
            for (int v : adj[u]) {
                // 只处理一次边 (u, v) 和 (v, u)
                if (u < v) {
                    int setU = find(parent, u);
                    int setV = find(parent, v);

                    if (setU == setV)
                        return true;

                    unionSets(parent, setU, setV);
                }
            }
        }

        return false;
    }

    // 并查集查找操作
    int find(vector<int>& parent, int i) {
        if (parent[i] == -1)
            return i;
        return find(parent, parent[i]);
    }

    // 并查集合并操作
    void unionSets(vector<int>& parent, int x, int y) {
        parent[x] = y;
    }

    // 有向图环检测 - 使用Kahn算法(拓扑排序)
    bool hasCycleDirectedKahn() {
        if (!directed)
            throw invalid_argument("该方法仅适用于有向图");

        vector<int> inDegree(V, 0);

        // 计算每个顶点的入度
        for (int u = 0; u < V; ++u) {
            for (int v : adj[u]) {
                inDegree[v]++;
            }
        }

        queue<int> q;
        for (int i = 0; i < V; ++i) {
            if (inDegree[i] == 0) {
                q.push(i);
            }
        }

        int count = 0;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            count++;

            for (int v : adj[u]) {
                if (--inDegree[v] == 0) {
                    q.push(v);
                }
            }
        }

        // 如果所有顶点都能被拓扑排序，则没有环
        return count != V;
    }

    // 检测哈密尔顿回路(包含所有顶点的环)
    bool hasHamiltonianCycle() {
        if (V < 2) return false; // 至少需要2个顶点

        vector<int> path;
        vector<bool> visited(V, false);

        // 尝试从每个顶点开始寻找哈密尔顿回路
        for (int i = 0; i < V; ++i) {
            path.clear();
            fill(visited.begin(), visited.end(), false);
            
            path.push_back(i);
            visited[i] = true;
            
            if (dfsHamiltonian(i, 1, path, visited)) {
                // 检查最后一个顶点是否能回到起点
                for (int neighbor : adj[path.back()]) {
                    if (neighbor == i) {
                        // 找到哈密尔顿回路
                        cout << "找到哈密尔顿回路: ";
                        for (int v : path) cout << v << " ";
                        cout << i << endl; // 回到起点
                        return true;
                    }
                }
            }
        }

        return false;
    }

    // 哈密尔顿回路DFS辅助函数
    bool dfsHamiltonian(int v, int depth, vector<int>& path, vector<bool>& visited) {
        // 如果已经访问了所有顶点，检查是否可以回到起点形成环
        if (depth == V) {
            return true;
        }

        // 尝试所有相邻顶点
        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                path.push_back(neighbor);
                visited[neighbor] = true;

                if (dfsHamiltonian(neighbor, depth + 1, path, visited))
                    return true;

                // 回溯
                path.pop_back();
                visited[neighbor] = false;
            }
        }

        return false;
    }

    // 打印图的邻接表
    void printGraph() {
        for (int i = 0; i < V; ++i) {
            cout << "顶点 " << i << ": ";
            for (int v : adj[i]) {
                cout << v << " ";
            }
            cout << endl;
        }
    }

    // 查找所有哈密尔顿回路
    vector<vector<int>> findAllHamiltonianCycles() {
        vector<vector<int>> result;
        if (V < 2) return result; // 至少需要2个顶点

        vector<int> path;
        vector<bool> visited(V, false);

        // 尝试从每个顶点开始寻找哈密尔顿回路
        for (int i = 0; i < V; ++i) {
            path.clear();
            fill(visited.begin(), visited.end(), false);
            
            path.push_back(i);
            visited[i] = true;
            
            vector<vector<int>> cyclesFromStart;
            dfsAllHamiltonian(i, 1, path, visited, cyclesFromStart, i);
            
            result.insert(result.end(), cyclesFromStart.begin(), cyclesFromStart.end());
        }

        // 对于无向图，可能存在重复的回路（方向相反但顶点序列相同）
        if (!directed) {
            removeDuplicateCycles(result);
        }

        return result;
    }

    // 查找所有哈密尔顿回路的DFS辅助函数
    void dfsAllHamiltonian(int v, int depth, vector<int>& path, vector<bool>& visited, 
                           vector<vector<int>>& cycles, int start) {
        // 如果已经访问了所有顶点，检查是否可以回到起点形成环
        if (depth == V) {
            // 检查最后一个顶点是否能回到起点
            for (int neighbor : adj[v]) {
                if (neighbor == start) {
                    // 找到哈密尔顿回路
                    vector<int> cycle = path;
                    cycle.push_back(start); // 添加回到起点的边
                    cycles.push_back(cycle);
                }
            }
            return;
        }

        // 尝试所有相邻顶点
        for (int neighbor : adj[v]) {
            if (!visited[neighbor]) {
                path.push_back(neighbor);
                visited[neighbor] = true;

                dfsAllHamiltonian(neighbor, depth + 1, path, visited, cycles, start);

                // 回溯
                path.pop_back();
                visited[neighbor] = false;
            }
        }
    }

    // 移除无向图中重复的哈密尔顿回路
    void removeDuplicateCycles(vector<vector<int>>& cycles) {
        for (auto it = cycles.begin(); it != cycles.end(); ++it) {
            // 复制当前回路并反转
            vector<int> reversed = *it;
            std::reverse(reversed.begin(), reversed.end());
            
            // 检查是否存在与反转后相同的回路
            auto jt = next(it);
            while (jt != cycles.end()) {
                if (*jt == reversed) {
                    jt = cycles.erase(jt);
                } else {
                    ++jt;
                }
            }
        }
    }

    // 打印所有哈密尔顿回路
    void printAllHamiltonianCycles() {
        vector<vector<int>> cycles = findAllHamiltonianCycles();
        
        if (cycles.empty()) {
            cout << "未找到哈密尔顿回路" << endl;
            return;
        }
        
        cout << "找到 " << cycles.size() << " 个哈密尔顿回路:" << endl;
        for (size_t i = 0; i < cycles.size(); ++i) {
            cout << "回路 " << i+1 << ": ";
            for (int v : cycles[i]) {
                cout << v << " ";
            }
            cout << endl;
        }
    }
};

void testDirectedGraph()
{
    Graph graph(16, true);
    for (int i = 0; i < 8; ++i)
    {
        // die 间5条边
        for (int j = 0; j < 5; ++j)
        {
            graph.addEdge(i, i + 8);
            graph.addEdge(i + 8, i);
        }
    }

    {
        graph.addEdge(0, 2);
        graph.addEdge(0, 4);
        graph.addEdge(0, 5);
        graph.addEdge(0, 7);
    }
    {
        graph.addEdge(1, 3);
        graph.addEdge(1, 4);
        graph.addEdge(1, 5);
        graph.addEdge(1, 6);
    }
    {
        graph.addEdge(2, 0);
        graph.addEdge(2, 5);
        graph.addEdge(2, 6);
        graph.addEdge(2, 7);
    }
    {
        graph.addEdge(3, 1);
        graph.addEdge(3, 4);
        graph.addEdge(3, 6);
        graph.addEdge(3, 7);
    }
    {
        graph.addEdge(4, 0);
        graph.addEdge(4, 1);
        graph.addEdge(4, 3);
        graph.addEdge(4, 6);
    }
    {
        graph.addEdge(5, 0);
        graph.addEdge(5, 1);
        graph.addEdge(5, 2);
        graph.addEdge(5, 7);
    }
    {
        graph.addEdge(6, 1);
        graph.addEdge(6, 2);
        graph.addEdge(6, 3);
        graph.addEdge(6, 4);
    }
    {
        graph.addEdge(7, 0);
        graph.addEdge(7, 2);
        graph.addEdge(7, 3);
        graph.addEdge(7, 3);
    }
    {
        graph.addEdge(8, 9);
        graph.addEdge(8, 11);
        graph.addEdge(8, 14);
    }
    {
        graph.addEdge(9, 8);
        graph.addEdge(9, 10);
        graph.addEdge(9, 15);
    }
    {
        graph.addEdge(10, 9);
        graph.addEdge(10, 11);
        graph.addEdge(10, 12);
    }
    {
        graph.addEdge(11, 10);
        graph.addEdge(11, 8);
        graph.addEdge(11, 13);
    }
    {
        graph.addEdge(12, 10);
        graph.addEdge(12, 13);
        graph.addEdge(12, 15);
    }
    {
        graph.addEdge(13, 11);
        graph.addEdge(13, 12);
        graph.addEdge(13, 14);
    }
    {
        graph.addEdge(14, 8);
        graph.addEdge(14, 13);
        graph.addEdge(14, 15);
    }
    {
        graph.addEdge(15, 9);
        graph.addEdge(15, 12);
        graph.addEdge(15, 14);
    }

    // 测试有向图环检测
    cout << "===== 有向图环检测 =====" << endl;
    cout << "有向图邻接表:" << endl;
    graph.printGraph();

    if (graph.hasCycleDirected())
        cout << "有向图中存在环 (DFS方法)" << endl;
    else
        cout << "有向图中不存在环 (DFS方法)" << endl;

    if (graph.hasCycleDirectedKahn())
        cout << "有向图中存在环 (Kahn方法)" << endl;
    else
        cout << "有向图中不存在环 (Kahn方法)" << endl;

}

void testUndirectedGraph()
{
    // 测试无向图环检测
    cout << "\n===== 无向图环检测 =====" << endl;
    Graph undirectedGraph(5, false);
    undirectedGraph.addEdge(0, 1);
    undirectedGraph.addEdge(1, 2);
    undirectedGraph.addEdge(2, 3);
    undirectedGraph.addEdge(3, 4);
    undirectedGraph.addEdge(4, 1); // 添加这条边会形成环

    cout << "无向图邻接表:" << endl;
    undirectedGraph.printGraph();

    try {
        if (undirectedGraph.hasCycleUndirected())
            cout << "无向图中存在环 (DFS方法)" << endl;
        else
            cout << "无向图中不存在环 (DFS方法)" << endl;

        if (undirectedGraph.hasCycleUndirectedUnionFind())
            cout << "无向图中存在环 (并查集方法)" << endl;
        else
            cout << "无向图中不存在环 (并查集方法)" << endl;
    } catch (const invalid_argument& e) {
        cout << "错误: " << e.what() << endl;
    }
}

void testHamiltonianCycle()
{
    Graph graph(16, true);
    for (int i = 0; i < 8; ++i)
    {
        // die 间5条边
        for (int j = 0; j < 5; ++j)
        {
            graph.addEdge(i, i + 8);
            graph.addEdge(i + 8, i);
        }
    }

    {
        graph.addEdge(0, 2);
        graph.addEdge(0, 4);
        graph.addEdge(0, 5);
        graph.addEdge(0, 7);
    }
    {
        graph.addEdge(1, 3);
        graph.addEdge(1, 4);
        graph.addEdge(1, 5);
        graph.addEdge(1, 6);
    }
    {
        graph.addEdge(2, 0);
        graph.addEdge(2, 5);
        graph.addEdge(2, 6);
        graph.addEdge(2, 7);
    }
    {
        graph.addEdge(3, 1);
        graph.addEdge(3, 4);
        graph.addEdge(3, 6);
        graph.addEdge(3, 7);
    }
    {
        graph.addEdge(4, 0);
        graph.addEdge(4, 1);
        graph.addEdge(4, 3);
        graph.addEdge(4, 6);
    }
    {
        graph.addEdge(5, 0);
        graph.addEdge(5, 1);
        graph.addEdge(5, 2);
        graph.addEdge(5, 7);
    }
    {
        graph.addEdge(6, 1);
        graph.addEdge(6, 2);
        graph.addEdge(6, 3);
        graph.addEdge(6, 4);
    }
    {
        graph.addEdge(7, 0);
        graph.addEdge(7, 2);
        graph.addEdge(7, 3);
        graph.addEdge(7, 3);
    }
    {
        graph.addEdge(8, 9);
        graph.addEdge(8, 11);
        graph.addEdge(8, 14);
    }
    {
        graph.addEdge(9, 8);
        graph.addEdge(9, 10);
        graph.addEdge(9, 15);
    }
    {
        graph.addEdge(10, 9);
        graph.addEdge(10, 11);
        graph.addEdge(10, 12);
    }
    {
        graph.addEdge(11, 10);
        graph.addEdge(11, 8);
        graph.addEdge(11, 13);
    }
    {
        graph.addEdge(12, 10);
        graph.addEdge(12, 13);
        graph.addEdge(12, 15);
    }
    {
        graph.addEdge(13, 11);
        graph.addEdge(13, 12);
        graph.addEdge(13, 14);
    }
    {
        graph.addEdge(14, 8);
        graph.addEdge(14, 13);
        graph.addEdge(14, 15);
    }
    {
        graph.addEdge(15, 9);
        graph.addEdge(15, 12);
        graph.addEdge(15, 14);
    }
    cout << "\n哈密尔顿图1邻接表:" << endl;
    graph.printGraph();
    // if (graph.hasHamiltonianCycle())
    //     cout << "哈密尔顿图1中存在哈密尔顿回路" << endl;
    // else
    //     cout << "哈密尔顿图1中不存在哈密尔顿回路" << endl;

    graph.printAllHamiltonianCycles();
}

int main() {
    testHamiltonianCycle();
    return 0;
}
