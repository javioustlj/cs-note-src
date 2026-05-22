#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <memory>

/**
 * Edge structure for flow network
 */
struct FlowEdge {
    int to, capacity, flow;
    FlowEdge* rev;
    
    FlowEdge(int t, int cap) : to(t), capacity(cap), flow(0), rev(nullptr) {}
};

/**
 * Flow Network class implementing Dinic's algorithm
 */
class FlowNetwork {
private:
    int n;
    std::vector<std::vector<FlowEdge*>> adj;
    std::vector<int> level;
    std::vector<int> iter;
    
public:
    FlowNetwork(int vertices) : n(vertices), adj(vertices), level(vertices), iter(vertices) {}
    
    ~FlowNetwork() {
        for (auto& row : adj) {
            for (auto edge : row) {
                delete edge;
            }
        }
    }
    
    void addEdge(int from, int to, int capacity) {
        FlowEdge* forward = new FlowEdge(to, capacity);
        FlowEdge* backward = new FlowEdge(from, 0);
        
        forward->rev = backward;
        backward->rev = forward;
        
        adj[from].push_back(forward);
        adj[to].push_back(backward);
    }
    
    bool bfs(int s, int t) {
        level.assign(n, -1);
        level[s] = 0;
        
        std::queue<int> q;
        q.push(s);
        
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            
            for (FlowEdge* edge : adj[v]) {
                if (edge->capacity - edge->flow > 0 && level[edge->to] < 0) {
                    level[edge->to] = level[v] + 1;
                    q.push(edge->to);
                }
            }
        }
        
        return level[t] >= 0;
    }
    
    int dfs(int v, int t, int f) {
        if (v == t) return f;
        
        for (int& i = iter[v]; i < adj[v].size(); i++) {
            FlowEdge* edge = adj[v][i];
            
            if (edge->capacity - edge->flow > 0 && level[v] < level[edge->to]) {
                int d = dfs(edge->to, t, std::min(f, edge->capacity - edge->flow));
                if (d > 0) {
                    edge->flow += d;
                    edge->rev->flow -= d;
                    return d;
                }
            }
        }
        
        return 0;
    }
    
    int maxFlow(int s, int t) {
        int flow = 0;
        
        while (bfs(s, t)) {
            iter.assign(n, 0);
            
            while (int f = dfs(s, t, 1e9)) {
                flow += f;
            }
        }
        
        return flow;
    }
    
    std::vector<std::vector<int>> extractPaths(int s, int t) {
        std::vector<std::vector<int>> paths;
        std::vector<bool> visited(n, false);
        
        for (FlowEdge* edge : adj[s]) {
            if (edge->flow > 0) {
                std::vector<int> path;
                findPath(edge->to, t, path, visited);
                if (!path.empty()) {
                    paths.push_back(path);
                    edge->flow--;
                }
            }
        }
        
        return paths;
    }
    
private:
    bool findPath(int v, int t, std::vector<int>& path, std::vector<bool>& visited) {
        if (v == t) {
            path.push_back(v);
            return true;
        }
        
        if (visited[v]) return false;
        
        visited[v] = true;
        path.push_back(v);
        
        for (FlowEdge* edge : adj[v]) {
            if (edge->flow > 0) {
                edge->flow--;
                if (findPath(edge->to, t, path, visited)) {
                    return true;
                }
            }
        }
        
        path.pop_back();
        return false;
    }
};

/**
 * Main class for finding maximum set of edge-disjoint Hamiltonian cycles
 */
class EdgeDisjointHamiltonianFinder { 
private:
    int** adjMatrix;
    int numVertices;
    std::vector<std::vector<int>> edgeDisjointCycles;
    
public:
    EdgeDisjointHamiltonianFinder(int vertices) : numVertices(vertices) {
        adjMatrix = new int*[numVertices];
        for (int i = 0; i < numVertices; i++) {
            adjMatrix[i] = new int[numVertices];
            for (int j = 0; j < numVertices; j++) {
                adjMatrix[i][j] = 0;
            }
        }
    }
    
    ~EdgeDisjointHamiltonianFinder() {
        for (int i = 0; i < numVertices; i++) {
            delete[] adjMatrix[i];
        }
        delete[] adjMatrix;
    }
    
    void addEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices && u != v) {
            adjMatrix[u][v] = 1;
            adjMatrix[v][u] = 1;
        }
    }
    
    void removeEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            adjMatrix[u][v] = 0;
            adjMatrix[v][u] = 0;
        }
    }
    
    bool hasEdge(int u, int v) const {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            return adjMatrix[u][v] == 1;
        }
        return false;
    }
    
    void findMaxEdgeDisjointHamiltonianCycles() {
        edgeDisjointCycles.clear();
        
        // Build flow network
        int flowNodes = 2 * numVertices + 2; // left + right + source + sink
        int source = 2 * numVertices;
        int sink = 2 * numVertices + 1;
        
        FlowNetwork flowNet(flowNodes);
        
        // Add edges from source to left partition
        for (int i = 0; i < numVertices; i++) {
            flowNet.addEdge(source, i, 1);
        }
        
        // Add edges from right partition to sink
        for (int i = 0; i < numVertices; i++) {
            flowNet.addEdge(numVertices + i, sink, 1);
        }
        
        // Add edges between partitions based on original graph
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                if (hasEdge(i, j)) {
                    flowNet.addEdge(i, numVertices + j, 1);
                }
            }
        }
        
        // Find maximum flow
        int maxFlow = flowNet.maxFlow(source, sink);
        
        // Extract cycles from flow
        extractCyclesFromFlow(flowNet, source, sink);
    }
    
    void printEdgeDisjointCycles() const {
        if (edgeDisjointCycles.empty()) {
            std::cout << "No edge-disjoint Hamiltonian cycles found.\n";
            return;
        }
        
        std::cout << "Found " << edgeDisjointCycles.size() 
                  << " edge-disjoint Hamiltonian cycles:\n";
        
        for (size_t i = 0; i < edgeDisjointCycles.size(); i++) {
            std::cout << "Cycle " << (i + 1) << ": ";
            const auto& cycle = edgeDisjointCycles[i];
            
            for (size_t j = 0; j < cycle.size(); j++) {
                std::cout << cycle[j];
                if (j < cycle.size() - 1) {
                    std::cout << " -> ";
                }
            }
            std::cout << " -> " << cycle[0] << "\n";
        }
        
        // Calculate total unique edges used
        std::set<std::pair<int, int>> allUsedEdges;
        for (const auto& cycle : edgeDisjointCycles) {
            for (size_t i = 0; i < cycle.size(); i++) {
                int u = cycle[i];
                int v = cycle[(i + 1) % cycle.size()];
                if (u < v) {
                    allUsedEdges.insert({u, v});
                } else {
                    allUsedEdges.insert({v, u});
                }
            }
        }
        
        std::cout << "Total unique edges used: " << allUsedEdges.size() << "\n";
    }
    
    void printAdjacencyMatrix() const {
        std::cout << "Adjacency Matrix:\n";
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                std::cout << adjMatrix[i][j] << " ";
            }
            std::cout << "\n";
        }
    }
    
    int getCycleCount() const {
        return edgeDisjointCycles.size();
    }
    
    // Helper methods for building specific graph structures
    void buildCompleteGraph() {
        for (int i = 0; i < numVertices; i++) {
            for (int j = i + 1; j < numVertices; j++) {
                addEdge(i, j);
            }
        }
    }
    
    void buildCycleGraph() {
        for (int i = 0; i < numVertices; i++) {
            addEdge(i, (i + 1) % numVertices);
        }
    }
    
    void buildBipartiteGraph() {
        if (numVertices % 2 != 0) {
            std::cout << "Warning: Bipartite graph requires even number of vertices\n";
            return;
        }
        
        int half = numVertices / 2;
        for (int i = 0; i < half; i++) {
            for (int j = half; j < numVertices; j++) {
                addEdge(i, j);
            }
        }
    }

    void addCompleteSubgraph(int vertices[], int size) {
        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                addEdge(vertices[i], vertices[j]);
            }
        }
    }

    void buildBr8Graph() {
        if (numVertices != 8) {
            std::cout << "Warning: Br8 graph requires exactly 8 vertices\n";
            return;
        }
        for (int i = 0; i < 4; i++) {
            addEdge(i, i + 4);
        }
        int group1[4] = {0, 1, 2, 3};
        addCompleteSubgraph(group1, 4);

        int group2[4] = {4, 5, 6, 7};
        addCompleteSubgraph(group2, 4);
    }

    void buildBr8Graph_OCS_6p() {
        if (numVertices != 8) {
            std::cout << "Warning: Br8Graph_OCS_6p requires exactly 8 vertices\n";
            return;
        }
        
        // Build complete graph first
        buildCompleteGraph();
        
        // Remove perfect matching edges
        removeEdge(0, 4);
        removeEdge(1, 5);
        removeEdge(2, 6);
        removeEdge(3, 7);
    }

    // Helper function to build a Br8Graph_OCS_6p subgraph starting from baseVertex
    // Uses only addEdge calls, no removeEdge calls
    void buildBr8Subgraph_OCS_6p(int baseVertex) {
        if (baseVertex + 7 >= numVertices) {
            std::cout << "Warning: Subgraph starting from vertex " << baseVertex << " exceeds vertex count\n";
            return;
        }
        
        // Add all edges within the 8-vertex subgraph except perfect matching edges
        // Perfect matching pairs to skip: (baseVertex+0, baseVertex+4), (baseVertex+1, baseVertex+5),
        // (baseVertex+2, baseVertex+6), (baseVertex+3, baseVertex+7)
        
        for (int i = 0; i < 8; i++) {
            for (int j = i + 1; j < 8; j++) {
                int u = baseVertex + i;
                int v = baseVertex + j;
                
                // Skip perfect matching edges
                bool isPerfectMatching = false;
                if ((i == 0 && j == 4) || (i == 1 && j == 5) ||
                    (i == 2 && j == 6) || (i == 3 && j == 7)) {
                    isPerfectMatching = true;
                }
                
                if (!isPerfectMatching) {
                    addEdge(u, v);
                }
            }
        }
    }

    void buildBr32Graph_OCS_6p() {
        if (numVertices != 32) {
            std::cout << "Warning: Br32Graph_OCS_6p requires exactly 32 vertices\n";
            return;
        }
        
        // Build 4 Br8Graph_OCS_6p subgraphs using only addEdge calls
        // Subgraph 1: vertices 0-7
        buildBr8Subgraph_OCS_6p(0);
        
        // Subgraph 2: vertices 8-15
        buildBr8Subgraph_OCS_6p(8);
        
        // Subgraph 3: vertices 16-23
        buildBr8Subgraph_OCS_6p(16);
        
        // Subgraph 4: vertices 24-31
        buildBr8Subgraph_OCS_6p(24);
        
        // Add inter-subgraph complete connections
        // Connect corresponding vertices across all 4 subgraphs as complete subgraphs K4
        for (int i = 0; i < 8; i++) {
            int vertices[4] = {i, i + 8, i + 16, i + 24};
            addCompleteSubgraph(vertices, 4);
        }
    }

    void buildBr64Graph_OCS_6p() {
        if (numVertices != 64) {
            std::cout << "Warning: Br64Graph_OCS_6p requires exactly 64 vertices\n";
            return;
        }
        
        // 8 buildBr8Subgraph_OCS_6p
        for (int i = 0; i < 8; ++i) {
            buildBr8Subgraph_OCS_6p(i * 8);
        }

        // 2 buildBr32Graph_OCS_6p
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 8; j++)
            {
                int vertices[4] = {i * 32, i * 32 + 8, i * 32 + 16, i * 32 + 24};
                addCompleteSubgraph(vertices, 4);
            }
        }

        for (int i = 0; i < 4 * 8; ++i) {
            addEdge(i, i + 32);
        }
    }
private:
    void extractCyclesFromFlow(FlowNetwork& flowNet, int source, int sink) {
        std::cout << "Extracting cycles from flow network...\n";
        
        // 1. 从流网络中提取边不相交路径
        auto flowPaths = flowNet.extractPaths(source, sink);
        std::cout << "Found " << flowPaths.size() << " flow paths\n";
        
        // 2. 将流路径转换为哈密顿环
        convertFlowPathsToHamiltonianCycles(flowPaths);
        
        // 3. 如果需要更多环，使用优化的回溯
        if (edgeDisjointCycles.size() < getMaxPossibleCycles()) {
            std::cout << "Using optimized backtracking for additional cycles...\n";
            findAdditionalCyclesWithBacktracking();
        }
        
        std::cout << "Total edge-disjoint Hamiltonian cycles found: " << edgeDisjointCycles.size() << "\n";
    }
    
    void convertFlowPathsToHamiltonianCycles(const std::vector<std::vector<int>>& flowPaths) {
        std::set<std::pair<int, int>> usedEdges;
        
        for (const auto& path : flowPaths) {
            // 流路径格式：source -> left_vertex -> right_vertex -> sink
            // 需要转换为原图中的哈密顿环
            
            std::vector<int> hamiltonianCycle;
            std::set<int> visitedInCycle;
            
            // 从路径中提取顶点序列
            for (size_t i = 1; i < path.size() - 1; i += 2) {
                int leftVertex = path[i];
                int rightVertex = path[i + 1];
                
                if (leftVertex < numVertices && !visitedInCycle.count(leftVertex)) {
                    hamiltonianCycle.push_back(leftVertex);
                    visitedInCycle.insert(leftVertex);
                }
            }
            
            // 检查是否形成有效哈密顿环
            if (hamiltonianCycle.size() == numVertices && isValidHamiltonianCycle(hamiltonianCycle, usedEdges)) {
                edgeDisjointCycles.push_back(hamiltonianCycle);
                
                // 标记使用的边
                for (size_t i = 0; i < hamiltonianCycle.size(); i++) {
                    int u = hamiltonianCycle[i];
                    int v = hamiltonianCycle[(i + 1) % hamiltonianCycle.size()];
                    usedEdges.insert(std::minmax(u, v));
                }
                std::cout << "Successfully converted flow path to Hamiltonian cycle\n";
            }
        }
    }
    
    bool isValidHamiltonianCycle(const std::vector<int>& cycle, const std::set<std::pair<int, int>>& usedEdges) {
        if (cycle.size() != numVertices) return false;
        
        // 检查所有顶点是否唯一
        std::set<int> vertices(cycle.begin(), cycle.end());
        if (vertices.size() != numVertices) return false;
        
        // 检查边是否存在且未被使用
        for (size_t i = 0; i < cycle.size(); i++) {
            int u = cycle[i];
            int v = cycle[(i + 1) % cycle.size()];
            
            if (!hasEdge(u, v)) return false;
            
            auto edge = std::minmax(u, v);
            if (usedEdges.find(edge) != usedEdges.end()) return false;
        }
        
        return true;
    }
    
    int getMaxPossibleCycles() {
        // 每个哈密顿环使用V条边，每条边只能用一次
        // 最大环数 = floor(总边数 / V)
        int totalEdges = 0;
        for (int i = 0; i < numVertices; i++) {
            for (int j = i + 1; j < numVertices; j++) {
                if (hasEdge(i, j)) totalEdges++;
            }
        }
        return totalEdges / numVertices;
    }
    
    void findAdditionalCyclesWithBacktracking() {
        std::set<std::pair<int, int>> usedEdges;
        
        // 标记已使用的边
        for (const auto& cycle : edgeDisjointCycles) {
            for (size_t i = 0; i < cycle.size(); i++) {
                int u = cycle[i];
                int v = cycle[(i + 1) % cycle.size()];
                usedEdges.insert(std::minmax(u, v));
            }
        }
        
        // 限制搜索时间和次数
        int maxAttempts = std::min(3, getMaxPossibleCycles() - (int)edgeDisjointCycles.size());
        int attempts = 0;
        
        std::vector<bool> visited(numVertices, false);
        std::vector<int> currentPath;
        
        for (int start = 0; start < numVertices && attempts < maxAttempts; start++) {
            if (findHamiltonianCycleWithEdgeConstraints(start, visited, currentPath, usedEdges)) {
                attempts++;
                visited.assign(numVertices, false);
                currentPath.clear();
                std::cout << "Found additional cycle " << attempts << " using backtracking\n";
            }
        }
    }
    
    bool findHamiltonianCycleWithEdgeConstraints(int start, std::vector<bool>& visited,
                                               std::vector<int>& currentPath,
                                               std::set<std::pair<int, int>>& usedEdges) {
        currentPath.push_back(start);
        visited[start] = true;
        
        if (currentPath.size() == numVertices) {
            // Check if we can return to start
            int last = currentPath.back();
            if (hasEdge(last, currentPath[0])) {
                std::pair<int, int> edge = std::minmax(last, currentPath[0]);
                if (usedEdges.find(edge) == usedEdges.end()) {
                    // Found a valid cycle
                    edgeDisjointCycles.push_back(currentPath);
                    
                    // Mark all edges in this cycle as used
                    for (size_t i = 0; i < currentPath.size(); i++) {
                        int u = currentPath[i];
                        int v = currentPath[(i + 1) % currentPath.size()];
                        usedEdges.insert(std::minmax(u, v));
                    }
                    return true;
                }
            }
            visited[start] = false;
            currentPath.pop_back();
            return false;
        }
        
        for (int next = 0; next < numVertices; next++) {
            if (!visited[next] && hasEdge(start, next)) {
                std::pair<int, int> edge = std::minmax(start, next);
                if (usedEdges.find(edge) == usedEdges.end()) {
                    if (findHamiltonianCycleWithEdgeConstraints(next, visited, currentPath, usedEdges)) {
                        return true;
                    }
                }
            }
        }
        
        visited[start] = false;
        currentPath.pop_back();
        return false;
    }
};

void testBr8Graph_OCS_6p() {
    std::cout << "Test Case: Br8Graph_OCS_6p (K8 minus perfect matching)\n";
    std::cout << "========================================================\n";
    
    EdgeDisjointHamiltonianFinder finder(8);
    finder.buildBr8Graph_OCS_6p();
    
    std::cout << "Adjacency Matrix:\n";
    finder.printAdjacencyMatrix();
    
    finder.findMaxEdgeDisjointHamiltonianCycles();
    finder.printEdgeDisjointCycles();
    
    std::cout << "Test Completed\n";
    std::cout << "========================================================\n\n";
}

void testOriginalBr8Graph() {
    std::cout << "Test Case: Original Br8Graph\n";
    std::cout << "========================================\n";
    
    EdgeDisjointHamiltonianFinder finder(8);
    finder.buildBr8Graph();
    finder.printAdjacencyMatrix();
    finder.findMaxEdgeDisjointHamiltonianCycles();
    finder.printEdgeDisjointCycles();
    
    std::cout << "Test Completed\n";
    std::cout << "========================================\n\n";
}

void testBr32Graph_OCS_6p() {
    std::cout << "Test Case: Br32Graph_OCS_6p (4 Br8Graph_OCS_6p subgraphs with interconnections)\n";
    std::cout << "================================================================================\n";
    
    EdgeDisjointHamiltonianFinder finder(32);
    finder.buildBr32Graph_OCS_6p();
    
    std::cout << "Graph structure: 32 vertices composed of 4 Br8Graph_OCS_6p subgraphs\n";
    std::cout << "Subgraphs: 0-7, 8-15, 16-23, 24-31\n";
    std::cout << "Interconnections: Complete subgraphs between corresponding vertices\n";
    std::cout << "               (0-8-16-24), (1-9-17-25), ..., (7-15-23-31)\n\n";
    
    // Print a smaller version of adjacency matrix for readability
    std::cout << "Adjacency Matrix (showing first 8x8 block for subgraph 0-7):\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << (finder.hasEdge(i, j) ? "1" : "0") << " ";
        }
        std::cout << "\n";
    }
    
    std::cout << "\nChecking inter-subgraph connections (sample):\n";
    std::cout << "Edge 0-8: " << (finder.hasEdge(0, 8) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 0-16: " << (finder.hasEdge(0, 16) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 0-24: " << (finder.hasEdge(0, 24) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 1-9: " << (finder.hasEdge(1, 9) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 7-31: " << (finder.hasEdge(7, 31) ? "EXISTS" : "MISSING") << "\n";
    
    std::cout << "\nChecking removed perfect matching edges (should be missing):\n";
    std::cout << "Edge 0-4: " << (finder.hasEdge(0, 4) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 8-12: " << (finder.hasEdge(8, 12) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 16-20: " << (finder.hasEdge(16, 20) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 24-28: " << (finder.hasEdge(24, 28) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    
    finder.findMaxEdgeDisjointHamiltonianCycles();
    finder.printEdgeDisjointCycles();
    
    std::cout << "Test Completed\n";
    std::cout << "================================================================================\n\n";
}

void testBr64Graph_OCS_6p() {
    std::cout << "Test Case: Br64Graph_OCS_6p (8 Br8Graph_OCS_6p subgraphs with complex interconnections)\n";
    std::cout << "==========================================================================================\n";
    
    EdgeDisjointHamiltonianFinder finder(64);
    finder.buildBr64Graph_OCS_6p();
    
    std::cout << "Graph structure: 64 vertices composed of 8 Br8Graph_OCS_6p subgraphs\n";
    std::cout << "Subgraphs: 0-7, 8-15, 16-23, 24-31, 32-39, 40-47, 48-55, 56-63\n";
    std::cout << "Interconnections: Complete subgraphs between corresponding vertices across all 8 subgraphs\n";
    std::cout << "               (0-8-16-24-32-40-48-56), (1-9-17-25-33-41-49-57), ..., (7-15-23-31-39-47-55-63)\n";
    std::cout << "Cross-connections: Edges between vertices 0-31 and 32-63\n\n";
    
    // Print a smaller version of adjacency matrix for readability
    std::cout << "Adjacency Matrix (showing first 8x8 block for subgraph 0-7):\n";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << (finder.hasEdge(i, j) ? "1" : "0") << " ";
        }
        std::cout << "\n";
    }
    
    std::cout << "\nChecking inter-subgraph connections (sample):\n";
    std::cout << "Edge 0-8: " << (finder.hasEdge(0, 8) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 0-16: " << (finder.hasEdge(0, 16) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 0-24: " << (finder.hasEdge(0, 24) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 0-32: " << (finder.hasEdge(0, 32) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 7-63: " << (finder.hasEdge(7, 63) ? "EXISTS" : "MISSING") << "\n";
    
    std::cout << "\nChecking removed perfect matching edges (should be missing):\n";
    std::cout << "Edge 0-4: " << (finder.hasEdge(0, 4) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 8-12: " << (finder.hasEdge(8, 12) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 16-20: " << (finder.hasEdge(16, 20) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 24-28: " << (finder.hasEdge(24, 28) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 32-36: " << (finder.hasEdge(32, 36) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 40-44: " << (finder.hasEdge(40, 44) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 48-52: " << (finder.hasEdge(48, 52) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    std::cout << "Edge 56-60: " << (finder.hasEdge(56, 60) ? "EXISTS (ERROR)" : "MISSING (CORRECT)") << "\n";
    
    std::cout << "\nChecking cross-connections between halves:\n";
    std::cout << "Edge 0-32: " << (finder.hasEdge(0, 32) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 15-47: " << (finder.hasEdge(15, 47) ? "EXISTS" : "MISSING") << "\n";
    std::cout << "Edge 31-63: " << (finder.hasEdge(31, 63) ? "EXISTS" : "MISSING") << "\n";
    
    finder.findMaxEdgeDisjointHamiltonianCycles();
    finder.printEdgeDisjointCycles();
    
    std::cout << "Test Completed\n";
    std::cout << "==========================================================================================\n\n";
}

int main() {
    std::cout << "Edge-Disjoint Hamiltonian Cycles Finder\n";
    std::cout << "========================================\n\n";
    
    // Test original Br8Graph
    testOriginalBr8Graph();
    
    // Test new Br8Graph_OCS_6p
    testBr8Graph_OCS_6p();
    
    // Test new Br32Graph_OCS_6p
    testBr32Graph_OCS_6p();
    
    // Test new Br64Graph_OCS_6p
    testBr64Graph_OCS_6p();
    
    return 0;
}
