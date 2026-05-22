#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <queue>
#include <chrono>
#include <random>
#include <memory>

/**
 * Optimized Undirected graph class using adjacency list representation
 * Specifically designed to find edge-disjoint Hamiltonian cycles efficiently
 */
class OptimizedGraph {
private:
    // Adjacency list for memory efficiency
    std::vector<std::vector<int>> adjacencyList;
    int numVertices;

    // Bitset for fast visited tracking (supports up to 1024 vertices)
    static constexpr int MAX_VERTICES = 1024;
    using VisitedBitset = std::bitset<MAX_VERTICES>;

    // Edge representation for efficient operations
    struct Edge {
        int u, v;
        Edge(int u_, int v_) : u(std::min(u_, v_)), v(std::max(u_, v_)) {}
        bool operator==(const Edge& other) const {
            return u == other.u && v == other.v;
        }
        bool operator<(const Edge& other) const {
            return u < other.u || (u == other.u && v < other.v);
        }
    };

    // Results storage
    std::vector<std::vector<int>> maxEdgeDisjointCycles;
    std::vector<std::vector<int>> allHamiltonianCycles;
    std::set<std::vector<int>> cycleSet;  // For deduplication

    // Performance optimization structures
    std::vector<int> vertexDegrees;
    std::vector<bool> connectivityCache;

public:
    /**
     * Constructor
     * @param vertices Number of vertices
     */
    OptimizedGraph(int vertices) : numVertices(vertices) {
        if (vertices > MAX_VERTICES) {
            throw std::invalid_argument("Graph size exceeds maximum supported vertices");
        }

        adjacencyList.resize(numVertices);
        vertexDegrees.resize(numVertices, 0);
        connectivityCache.resize(numVertices * numVertices, false);
    }

    /**
     * Add undirected edge
     */
    void addEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices && u != v) {
            adjacencyList[u].push_back(v);
            adjacencyList[v].push_back(u);
            vertexDegrees[u]++;
            vertexDegrees[v]++;

            // Invalidate connectivity cache
            invalidateConnectivityCache(u, v);
        }
    }

    /**
     * Remove undirected edge
     */
    void removeEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            auto& listU = adjacencyList[u];
            auto& listV = adjacencyList[v];

            listU.erase(std::remove(listU.begin(), listU.end(), v), listU.end());
            listV.erase(std::remove(listV.begin(), listV.end(), u), listV.end());

            vertexDegrees[u]--;
            vertexDegrees[v]--;

            // Invalidate connectivity cache
            invalidateConnectivityCache(u, v);
        }
    }

    /**
     * Check if there is an edge between two vertices
     */
    bool hasEdge(int u, int v) const {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            return std::find(adjacencyList[u].begin(), adjacencyList[u].end(), v) != adjacencyList[u].end();
        }
        return false;
    }

    /**
     * Get number of vertices
     */
    int getVertexCount() const {
        return numVertices;
    }

    /**
     * Get degree of a vertex
     */
    int getDegree(int vertex) const {
        if (vertex >= 0 && vertex < numVertices) {
            return vertexDegrees[vertex];
        }
        return 0;
    }

    /**
     * Main optimized function to find maximum edge-disjoint Hamiltonian cycles
     */
    void findMaxEdgeDisjointHamiltonianCycles() {
        maxEdgeDisjointCycles.clear();
        allHamiltonianCycles.clear();
        cycleSet.clear();

        // First, find all Hamiltonian cycles
        findAllHamiltonianCycles();

        if (allHamiltonianCycles.empty()) {
            std::cout << "No Hamiltonian cycles found in the graph.\n";
            return;
        }

        // Then find the maximum edge-disjoint set
        std::vector<std::vector<int>> currentSolution;
        std::set<Edge> usedEdges;
        findMaxEdgeDisjointCyclesUtil(0, currentSolution, usedEdges, maxEdgeDisjointCycles);
    }


    /**
     * Print maximum edge-disjoint Hamiltonian cycles
     */
    void printMaxEdgeDisjointCycles() const {
        if (maxEdgeDisjointCycles.empty()) {
            std::cout << "No edge-disjoint Hamiltonian cycles found.\n";
            return;
        }

        std::cout << "Found maximum set of " << maxEdgeDisjointCycles.size()
                  << " edge-disjoint Hamiltonian cycles:\n";

        // Separate repeatable and non-repeatable edges for statistics
        std::set<Edge> repeatableEdges;
        std::set<Edge> nonRepeatableEdges;

        for (const auto& cycle : maxEdgeDisjointCycles) {
            auto edges = extractEdgesFromCycle(cycle);
            for (const auto& edge : edges) {
                if (isEdgeRepeatable(edge.u, edge.v)) {
                    repeatableEdges.insert(edge);
                } else {
                    nonRepeatableEdges.insert(edge);
                }
            }
        }

        for (size_t i = 0; i < maxEdgeDisjointCycles.size(); i++) {
            std::cout << "Edge-Disjoint Cycle " << (i + 1) << ": ";
            printCycle(maxEdgeDisjointCycles[i]);
        }

        std::cout << "Total unique non-repeatable edges used: " << nonRepeatableEdges.size() << "\n";
        std::cout << "Total unique repeatable edges used: " << repeatableEdges.size() << "\n";
        std::cout << "Total unique edges used: " << (nonRepeatableEdges.size() + repeatableEdges.size()) << "\n";

        // Show which repeatable edges were used
        if (!repeatableEdges.empty()) {
            std::cout << "Repeatable edges used: ";
            bool first = true;
            for (const auto& edge : repeatableEdges) {
                if (!first) std::cout << ", ";
                std::cout << "(" << edge.u << "," << edge.v << ")";
                first = false;
            }
            std::cout << "\n";
        }
    }

    /**
     * Get number of found cycles
     */
    int getCycleCount() const {
        return maxEdgeDisjointCycles.size();
    }

    /**
     * Print adjacency list (for debugging)
     */
    void printAdjacencyList() const {
        std::cout << "Adjacency List:\n";
        for (int i = 0; i < numVertices; i++) {
            std::cout << i << ": ";
            for (int neighbor : adjacencyList[i]) {
                std::cout << neighbor << " ";
            }
            std::cout << "\n";
        }
    }

    /**
     * Build the same graph structures as original for testing
     */
    void buildGraph() {
        if (numVertices != 8) {
            std::cout << "Error: buildGraph() requires exactly 8 vertices\n";
            return;
        }

        // Create complete subgraph for vertices 0,1,2,3
        for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 4; j++) {
                addEdge(i, j);
            }
        }

        // Create complete subgraph for vertices 4,5,6,7
        for (int i = 4; i < 8; i++) {
            for (int j = i + 1; j < 8; j++) {
                addEdge(i, j);
            }
        }

        // Add bridge edges
        addEdge(0, 4);
        addEdge(1, 5);
        addEdge(2, 6);
        addEdge(3, 7);
    }

    void buildGraph16() {
        if (numVertices != 16) {
            std::cout << "Error: buildGraph16() requires exactly 16 vertices\n";
            return;
        }

        // Sequential edges
        for (int i = 0; i < 15; i += 2) {
            addEdge(i, i + 1);
        }

        // Complete subgraphs
        int groupA[] = {0, 2, 4, 6};
        int groupB[] = {1, 3, 5, 7};
        int groupC[] = {8, 10, 12, 14};
        int groupD[] = {9, 11, 13, 15};

        addCompleteSubgraph(groupA, 4);
        addCompleteSubgraph(groupB, 4);
        addCompleteSubgraph(groupC, 4);
        addCompleteSubgraph(groupD, 4);

        // Cross connections
        addEdge(0, 8);
        addEdge(1, 9);
        addEdge(2, 10);
        addEdge(3, 11);
        addEdge(4, 12);
        addEdge(5, 13);
        addEdge(6, 14);
        addEdge(7, 15);
    }

private:
    /**
     * Add complete subgraph
     */
    void addCompleteSubgraph(int vertices[], int size) {
        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                addEdge(vertices[i], vertices[j]);
            }
        }
    }

    /**
     * Invalidate connectivity cache for affected vertices
     */
    void invalidateConnectivityCache(int u, int v) {
        for (int i = 0; i < numVertices; i++) {
            connectivityCache[u * numVertices + i] = false;
            connectivityCache[i * numVertices + u] = false;
            connectivityCache[v * numVertices + i] = false;
            connectivityCache[i * numVertices + v] = false;
        }
    }


    /**
     * Check if an edge is repeatable based on the rules:
     * 1. Edge (0,1) can be repeated
     * 2. Edge (2,3) can be repeated
     * 3. For any even n, edge (n, n+1) can be repeated
     */
    bool isEdgeRepeatable(int u, int v) const {
        // Normalize edge representation (smaller first)
        int minVertex = std::min(u, v);
        int maxVertex = std::max(u, v);

        // Rule 1: Edge (0,1) can be repeated
        if (minVertex == 0 && maxVertex == 1) return true;

        // Rule 2: Edge (2,3) can be repeated
        if (minVertex == 2 && maxVertex == 3) return true;

        // Rule 3: For any even n, edge (n, n+1) can be repeated
        if (minVertex % 2 == 0 && maxVertex == minVertex + 1) return true;

        return false;
    }



    /**
     * Extract edges from a cycle
     */
    std::set<Edge> extractEdgesFromCycle(const std::vector<int>& cycle) const {
        std::set<Edge> edges;
        for (size_t i = 0; i < cycle.size(); i++) {
            int u = cycle[i];
            int v = cycle[(i + 1) % cycle.size()];
            edges.emplace(u, v);
        }
        return edges;
    }

    /**
     * Find all Hamiltonian cycles
     */
    void findAllHamiltonianCycles() {
        allHamiltonianCycles.clear();
        cycleSet.clear();

        // Start search from each vertex
        for (int start = 0; start < numVertices; start++) {
            std::vector<int> path(numVertices, -1);
            VisitedBitset visited;

            path[0] = start;
            visited[start] = true;

            hamiltonianCycleUtil(path, visited, 1, start);
        }
    }

    /**
     * Recursive helper function for Hamiltonian cycle search
     */
    void hamiltonianCycleUtil(std::vector<int>& path, VisitedBitset& visited, int pos, int start) {
        // If all vertices are included in the path
        if (pos == numVertices) {
            // Check if it can form a cycle (last vertex connected to start vertex)
            if (hasEdge(path[pos - 1], start)) {
                std::vector<int> cycle = path;

                // Normalize cycle representation (start with minimum vertex to avoid duplicates)
                normalizeCycle(cycle);

                // Check if same cycle already exists
                if (cycleSet.find(cycle) == cycleSet.end()) {
                    cycleSet.insert(cycle);
                    allHamiltonianCycles.push_back(cycle);
                }
            }
            return;
        }

        // Try all possible next vertices
        for (int v = 0; v < numVertices; v++) {
            if (isSafe(v, pos, path, visited)) {
                path[pos] = v;
                visited[v] = true;

                // Recursive search
                hamiltonianCycleUtil(path, visited, pos + 1, start);

                // Backtrack
                visited[v] = false;
                path[pos] = -1;
            }
        }
    }

    /**
     * Check if vertex v can be added to the current position in the path
     */
    bool isSafe(int v, int pos, const std::vector<int>& path, const VisitedBitset& visited) {
        // Check if connected to previous vertex
        if (!hasEdge(path[pos - 1], v)) {
            return false;
        }

        // Check if already visited
        if (visited[v]) {
            return false;
        }

        return true;
    }

    /**
     * Normalize cycle representation to start with minimum vertex
     */
    void normalizeCycle(std::vector<int>& cycle) {
        // Find position of minimum vertex
        int minPos = 0;
        for (int i = 1; i < numVertices; i++) {
            if (cycle[i] < cycle[minPos]) {
                minPos = i;
            }
        }

        // Rearrange cycle so minimum vertex is at start
        std::vector<int> normalized;
        for (int i = 0; i < numVertices; i++) {
            normalized.push_back(cycle[(minPos + i) % numVertices]);
        }

        cycle = normalized;
    }

    /**
     * Backtracking utility to find maximum edge-disjoint cycles
     */
    void findMaxEdgeDisjointCyclesUtil(int currentIndex,
                                      std::vector<std::vector<int>>& currentSolution,
                                      std::set<Edge>& usedEdges,
                                      std::vector<std::vector<int>>& bestSolution) {
        // Update best solution if current is better
        if (currentSolution.size() > bestSolution.size()) {
            bestSolution = currentSolution;
        }

        // Try adding more cycles
        for (int i = currentIndex; i < allHamiltonianCycles.size(); i++) {
            const auto& cycle = allHamiltonianCycles[i];
            auto cycleEdges = extractEdgesFromCycle(cycle);

            // Check if this cycle overlaps with used non-repeatable edges
            if (!hasEdgeOverlapWithUsedEdges(usedEdges, cycleEdges)) {
                // Add this cycle to current solution
                currentSolution.push_back(cycle);

                // Add non-repeatable edges to used edges
                std::set<Edge> newUsedEdges = usedEdges;
                for (const auto& edge : cycleEdges) {
                    if (!isEdgeRepeatable(edge.u, edge.v)) {
                        newUsedEdges.insert(edge);
                    }
                }

                // Recurse
                findMaxEdgeDisjointCyclesUtil(i + 1, currentSolution, newUsedEdges, bestSolution);

                // Backtrack
                currentSolution.pop_back();
            }
        }
    }

    /**
     * Check if cycle edges overlap with used non-repeatable edges
     */
    bool hasEdgeOverlapWithUsedEdges(const std::set<Edge>& usedEdges, const std::set<Edge>& cycleEdges) const {
        for (const auto& edge : cycleEdges) {
            if (!isEdgeRepeatable(edge.u, edge.v) && usedEdges.find(edge) != usedEdges.end()) {
                return true;
            }
        }
        return false;
    }

    /**
     * Print a single cycle
     */
    void printCycle(const std::vector<int>& cycle) const {
        for (size_t i = 0; i < cycle.size(); i++) {
            std::cout << cycle[i];
            if (i < cycle.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << " -> " << cycle[0] << "\n";
    }
};

/**
 * Test functions matching the original
 */
void testCompleteGraphK4() {
    std::cout << "========================================\n";
    std::cout << "Test Case 1: Edge-disjoint Hamiltonian cycles on complete graph K4\n";
    std::cout << "========================================\n";

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Creating complete graph K4...\n";
    OptimizedGraph g1(4);

    std::cout << "Adding all edges to form complete graph K4...\n";
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            g1.addEdge(i, j);
        }
    }

    std::cout << "Displaying adjacency list:\n";
    g1.printAdjacencyList();

    std::cout << "Finding maximum edge-disjoint Hamiltonian cycles...\n";
    g1.findMaxEdgeDisjointHamiltonianCycles();

    std::cout << "Displaying maximum edge-disjoint Hamiltonian cycles:\n";
    g1.printMaxEdgeDisjointCycles();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms\n";

    std::cout << "Test Case 1 Completed\n";
    std::cout << "========================================\n\n";
}

void testSpecial8VertexGraphCombined() {
    std::cout << "========================================\n";
    std::cout << "Test Case 2: Special 8-vertex graph with comprehensive analysis\n";
    std::cout << "========================================\n";

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Creating 8-vertex graph...\n";
    OptimizedGraph g2(8);

    std::cout << "Building special graph structure (two K4 subgraphs with bridge edges)...\n";
    g2.buildGraph();

    std::cout << "Displaying adjacency list:\n";
    g2.printAdjacencyList();

    std::cout << "Finding maximum edge-disjoint Hamiltonian cycles...\n";
    g2.findMaxEdgeDisjointHamiltonianCycles();

    std::cout << "Displaying maximum edge-disjoint Hamiltonian cycles:\n";
    g2.printMaxEdgeDisjointCycles();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms\n";

    std::cout << "Test Case 2 Completed\n";
    std::cout << "========================================\n\n";
}

void test16VertexGraph() {
    std::cout << "========================================\n";
    std::cout << "Test Case 3: Special 16-vertex graph with complex structure\n";
    std::cout << "========================================\n";

    auto start = std::chrono::high_resolution_clock::now();

    std::cout << "Creating 16-vertex graph...\n";
    OptimizedGraph g4(16);

    std::cout << "Building special 16-vertex graph structure...\n";
    g4.buildGraph16();

    std::cout << "Displaying adjacency list:\n";
    g4.printAdjacencyList();

    std::cout << "Finding maximum edge-disjoint Hamiltonian cycles...\n";
    g4.findMaxEdgeDisjointHamiltonianCycles();

    std::cout << "Displaying maximum edge-disjoint Hamiltonian cycles:\n";
    g4.printMaxEdgeDisjointCycles();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms\n";

    std::cout << "Test Case 3 Completed\n";
    std::cout << "========================================\n\n";
}


/**
 * Main function
 */
int main() {
    std::cout << "Optimized Hamiltonian Cycle Finder Program\n";
    std::cout << "==========================================\n\n";
    
    testCompleteGraphK4();
    testSpecial8VertexGraphCombined();
    test16VertexGraph();

    
    return 0;
}
