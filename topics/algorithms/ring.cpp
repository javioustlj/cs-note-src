#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

/**
 * Undirected graph class using adjacency matrix representation
 * Specifically designed to find all Hamiltonian cycles (rings that use all vertices without repeating edges)
 */
class Graph {
private:
    int** adjacencyMatrix;  // Adjacency matrix
    int numVertices;        // Number of vertices
    std::vector<std::vector<int>> allCycles;  // Store all found cycles
    std::set<std::vector<int>> cycleSet;     // Set for deduplication
    std::vector<std::vector<int>> maxEdgeDisjointCycles;  // Store maximum edge-disjoint cycles

public:
    /**
     * Constructor
     * @param vertices Number of vertices
     */
    Graph(int vertices) : numVertices(vertices) {
        // Allocate memory for adjacency matrix
        adjacencyMatrix = new int*[numVertices];
        for (int i = 0; i < numVertices; i++) {
            adjacencyMatrix[i] = new int[numVertices];
            for (int j = 0; j < numVertices; j++) {
                adjacencyMatrix[i][j] = 0;  // Initialize to 0 (no edge)
            }
        }
    }

    /**
     * Destructor to free memory
     */
    ~Graph() {
        for (int i = 0; i < numVertices; i++) {
            delete[] adjacencyMatrix[i];
        }
        delete[] adjacencyMatrix;
    }

    /**
     * Add undirected edge
     * @param u Vertex u
     * @param v Vertex v
     */
    void addEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices && u != v) {
            adjacencyMatrix[u][v] = 1;
            adjacencyMatrix[v][u] = 1;  // Undirected graph, symmetric setting
        }
    }

    /**
     * Remove undirected edge
     * @param u Vertex u
     * @param v Vertex v
     */
    void removeEdge(int u, int v) {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            adjacencyMatrix[u][v] = 0;
            adjacencyMatrix[v][u] = 0;
        }
    }

    /**
     * Check if there is an edge between two vertices
     * @param u Vertex u
     * @param v Vertex v
     * @return true if edge exists, false otherwise
     */
    bool hasEdge(int u, int v) const {
        if (u >= 0 && u < numVertices && v >= 0 && v < numVertices) {
            return adjacencyMatrix[u][v] == 1;
        }
        return false;
    }

    /**
     * Get number of vertices
     * @return Number of vertices
     */
    int getVertexCount() const {
        return numVertices;
    }

    /**
     * Find all Hamiltonian cycles
     */
    void findAllHamiltonianCycles() {
        allCycles.clear();
        cycleSet.clear();

        // Start search from each vertex
        for (int start = 0; start < numVertices; start++) {
            std::vector<int> path(numVertices, -1);
            std::vector<bool> visited(numVertices, false);

            path[0] = start;
            visited[start] = true;

            hamiltonianCycleUtil(path, visited, 1, start);
        }
    }

    /**
     * Find maximum set of edge-disjoint Hamiltonian cycles
     */
    void findMaxEdgeDisjointHamiltonianCycles() {
        // First find all Hamiltonian cycles
        findAllHamiltonianCycles();

        if (allCycles.empty()) {
            std::cout << "No Hamiltonian cycles found in the graph.\n";
            return;
        }

        maxEdgeDisjointCycles.clear();
        std::vector<std::vector<int>> currentSolution;
        std::set<std::pair<int, int>> usedEdges;

        // Start backtracking to find maximum edge-disjoint cycles
        findMaxEdgeDisjointCyclesUtil(0, currentSolution, usedEdges, maxEdgeDisjointCycles);
    }

    /**
     * Print all found cycles
     */
    void printAllCycles() const {
        std::cout << "Found " << allCycles.size() << " Hamiltonian cycles:\n";
        for (size_t i = 0; i < allCycles.size(); i++) {
            std::cout << "Cycle " << (i + 1) << ": ";
            printCycle(allCycles[i]);
        }
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

        // Calculate total unique edges used
        std::set<std::pair<int, int>> allUsedEdges;
        for (const auto& cycle : maxEdgeDisjointCycles) {
            auto edges = extractEdgesFromCycle(cycle);
            allUsedEdges.insert(edges.begin(), edges.end());
        }

        for (size_t i = 0; i < maxEdgeDisjointCycles.size(); i++) {
            std::cout << "Edge-Disjoint Cycle " << (i + 1) << ": ";
            printCycle(maxEdgeDisjointCycles[i]);
        }

        std::cout << "Total unique edges used: " << allUsedEdges.size() << "\n";
    }

    /**
     * Get number of found cycles
     * @return Number of cycles
     */
    int getCycleCount() const {
        return allCycles.size();
    }

    /**
     * Print adjacency matrix (for debugging)
     */
    void printAdjacencyMatrix() const {
        std::cout << "Adjacency Matrix:\n";
        for (int i = 0; i < numVertices; i++) {
            for (int j = 0; j < numVertices; j++) {
                std::cout << adjacencyMatrix[i][j] << " ";
            }
            std::cout << "\n";
        }
    }

    /**
     * Build a specific graph structure with 8 vertices:
     * - First 4 vertices (0,1,2,3) form a complete subgraph
     * - Last 4 vertices (4,5,6,7) form a complete subgraph
     * - Bridge edges: 0-4, 1-5, 2-6, 3-7
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

        // Add bridge edges between the two subgraphs
        addEdge(0, 4);
        addEdge(1, 5);
        addEdge(2, 6);
        addEdge(3, 7);
    }

    /**
     * Build a specific graph structure with 16 vertices:
     * - Sequential edges: 0-1, 1-2, 2-3, ..., 14-15
     * - Group A: 0,2,4,6 form a complete subgraph
     * - Group B: 1,3,5,7 form a complete subgraph
     * - Group C: 8,10,12,14 form a complete subgraph
     * - Group D: 9,11,13,15 form a complete subgraph
     * - Cross connections: 0-8, 1-9, 2-10, 3-11, 4-12, 5-13, 6-14, 7-15
     */
    void buildGraph16() {
        if (numVertices != 16) {
            std::cout << "Error: buildGraph16() requires exactly 16 vertices\n";
            return;
        }

        // Add sequential edges: 0-1, 1-2, 2-3, ..., 14-15
        for (int i = 0; i < 15; i++) {
            addEdge(i, i + 1);
        }

        // Group A: 0,2,4,6 form a complete subgraph
        int groupA[] = {0, 2, 4, 6};
        addCompleteSubgraph(groupA, 4);

        // Group B: 1,3,5,7 form a complete subgraph
        int groupB[] = {1, 3, 5, 7};
        addCompleteSubgraph(groupB, 4);

        // Group C: 8,10,12,14 form a complete subgraph
        int groupC[] = {8, 10, 12, 14};
        addCompleteSubgraph(groupC, 4);

        // Group D: 9,11,13,15 form a complete subgraph
        int groupD[] = {9, 11, 13, 15};
        addCompleteSubgraph(groupD, 4);

        // Add cross connections: 0-8, 1-9, 2-10, 3-11, 4-12, 5-13, 6-14, 7-15
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
     * Helper function to add a complete subgraph for given vertices
     * @param vertices Array of vertex indices
     * @param size Size of the vertex array
     */
    void addCompleteSubgraph(int vertices[], int size) {
        for (int i = 0; i < size; i++) {
            for (int j = i + 1; j < size; j++) {
                addEdge(vertices[i], vertices[j]);
            }
        }
    }
    /**
     * Check if vertex v can be added to the current position in the path
     * @param v Vertex to check
     * @param pos Current position
     * @param path Current path
     * @param visited Visited vertices marker
     * @return true if can be added, false otherwise
     */
    bool isSafe(int v, int pos, const std::vector<int>& path, const std::vector<bool>& visited) {
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
     * Recursive helper function for Hamiltonian cycle search
     * @param path Current path
     * @param visited Visited vertices marker
     * @param pos Current position
     * @param start Starting vertex
     */
    void hamiltonianCycleUtil(std::vector<int>& path, std::vector<bool>& visited, int pos, int start) {
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
                    allCycles.push_back(cycle);
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
     * Normalize cycle representation to start with minimum vertex
     * @param cycle Cycle to normalize
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
     * Print single cycle
     * @param cycle Cycle to print
     */
    void printCycle(const std::vector<int>& cycle) const {
        for (size_t i = 0; i < cycle.size(); i++) {
            std::cout << cycle[i];
            if (i < cycle.size() - 1) {
                std::cout << " -> ";
            }
        }
        std::cout << " -> " << cycle[0] << "\n";  // Back to starting point
    }

    /**
     * Normalize edge representation (undirected edge)
     * @param u First vertex
     * @param v Second vertex
     * @return Normalized edge as {min, max}
     */
    std::pair<int, int> normalizeEdge(int u, int v) const {
        return (u < v) ? std::make_pair(u, v) : std::make_pair(v, u);
    }

    /**
     * Extract all edges from a cycle
     * @param cycle The cycle
     * @return Set of normalized edges
     */
    std::set<std::pair<int, int>> extractEdgesFromCycle(const std::vector<int>& cycle) const {
        std::set<std::pair<int, int>> edges;
        for (size_t i = 0; i < cycle.size(); i++) {
            int u = cycle[i];
            int v = cycle[(i + 1) % cycle.size()];  // Wrap around to first vertex
            edges.insert(normalizeEdge(u, v));
        }
        return edges;
    }

    /**
     * Check if two edge sets have any overlap
     * @param edges1 First edge set
     * @param edges2 Second edge set
     * @return true if there's overlap, false otherwise
     */
    bool hasEdgeOverlap(const std::set<std::pair<int, int>>& edges1,
                       const std::set<std::pair<int, int>>& edges2) const {
        for (const auto& edge : edges1) {
            if (edges2.find(edge) != edges2.end()) {
                return true;
            }
        }
        return false;
    }

    /**
     * Backtracking utility to find maximum edge-disjoint cycles
     * @param currentIndex Current index in allCycles being considered
     * @param currentSolution Current set of selected cycles
     * @param usedEdges Set of edges used by current solution
     * @param bestSolution Best solution found so far
     */
    void findMaxEdgeDisjointCyclesUtil(int currentIndex,
                                      std::vector<std::vector<int>>& currentSolution,
                                      std::set<std::pair<int, int>>& usedEdges,
                                      std::vector<std::vector<int>>& bestSolution) {
        // Update best solution if current is better
        if (currentSolution.size() > bestSolution.size()) {
            bestSolution = currentSolution;
        }

        // Try adding more cycles
        for (int i = currentIndex; i < allCycles.size(); i++) {
            const auto& cycle = allCycles[i];
            auto cycleEdges = extractEdgesFromCycle(cycle);

            // Check if this cycle overlaps with used edges
            if (!hasEdgeOverlap(usedEdges, cycleEdges)) {
                // Add this cycle to current solution
                currentSolution.push_back(cycle);

                // Add edges to used edges
                std::set<std::pair<int, int>> newUsedEdges = usedEdges;
                newUsedEdges.insert(cycleEdges.begin(), cycleEdges.end());

                // Recurse
                findMaxEdgeDisjointCyclesUtil(i + 1, currentSolution, newUsedEdges, bestSolution);

                // Backtrack
                currentSolution.pop_back();
            }
        }
    }
};

/**
 * Test Case 1: Edge-disjoint Hamiltonian cycles on complete graph K4
 */
void testCompleteGraphK4() {
    std::cout << "========================================\n";
    std::cout << "Test Case 1: Edge-disjoint Hamiltonian cycles on complete graph K4\n";
    std::cout << "========================================\n";

    std::cout << "Creating complete graph K4...\n";
    Graph g1(4);

    std::cout << "Adding all edges to form complete graph K4...\n";
    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            g1.addEdge(i, j);
        }
    }

    std::cout << "Displaying adjacency matrix:\n";
    g1.printAdjacencyMatrix();

    std::cout << "Finding all Hamiltonian cycles...\n";
    g1.findAllHamiltonianCycles();

    std::cout << "Displaying all found Hamiltonian cycles:\n";
    g1.printAllCycles();

    std::cout << "Finding maximum edge-disjoint Hamiltonian cycles...\n";
    g1.findMaxEdgeDisjointHamiltonianCycles();

    std::cout << "Displaying maximum edge-disjoint Hamiltonian cycles:\n";
    g1.printMaxEdgeDisjointCycles();

    std::cout << "Test Case 1 Completed\n";
    std::cout << "========================================\n\n";
}

/**
 * Test Case 2: Special 8-vertex graph with comprehensive analysis
 * Combines functionality: finds all Hamiltonian cycles and edge-disjoint cycles
 */
void testSpecial8VertexGraphCombined() {
    std::cout << "========================================\n";
    std::cout << "Test Case 2: Special 8-vertex graph with comprehensive analysis\n";
    std::cout << "========================================\n";

    std::cout << "Creating 8-vertex graph...\n";
    Graph g2(8);

    std::cout << "Building special graph structure (two K4 subgraphs with bridge edges)...\n";
    g2.buildGraph();

    std::cout << "Displaying adjacency matrix:\n";
    g2.printAdjacencyMatrix();

    std::cout << "\n=== Part 1: Finding all Hamiltonian cycles ===\n";
    std::cout << "Finding all Hamiltonian cycles...\n";
    g2.findAllHamiltonianCycles();

    std::cout << "Displaying all found Hamiltonian cycles:\n";
    g2.printAllCycles();

    std::cout << "\n=== Part 2: Finding maximum edge-disjoint Hamiltonian cycles ===\n";
    std::cout << "Finding maximum edge-disjoint Hamiltonian cycles...\n";
    g2.findMaxEdgeDisjointHamiltonianCycles();

    std::cout << "Displaying maximum edge-disjoint Hamiltonian cycles:\n";
    g2.printMaxEdgeDisjointCycles();

    std::cout << "Test Case 2 Completed\n";
    std::cout << "========================================\n\n";
}

/**
 * Test Case 4: Special 16-vertex graph with complex structure
 */
void test16VertexGraph() {
    std::cout << "========================================\n";
    std::cout << "Test Case 3: Special 16-vertex graph with complex structure\n";
    std::cout << "========================================\n";

    std::cout << "Creating 16-vertex graph...\n";
    Graph g4(16);

    std::cout << "Building special 16-vertex graph structure...\n";
    std::cout << "- Sequential edges: 0-1, 1-2, ..., 14-15\n";
    std::cout << "- Group A: {0,2,4,6} complete subgraph\n";
    std::cout << "- Group B: {1,3,5,7} complete subgraph\n";
    std::cout << "- Group C: {8,10,12,14} complete subgraph\n";
    std::cout << "- Group D: {9,11,13,15} complete subgraph\n";
    std::cout << "- Cross connections: 0-8, 1-9, 2-10, 3-11, 4-12, 5-13, 6-14, 7-15\n";
    g4.buildGraph16();

    std::cout << "\nDisplaying adjacency matrix:\n";
    g4.printAdjacencyMatrix();

    std::cout << "\nFinding all Hamiltonian cycles...\n";
    g4.findAllHamiltonianCycles();

    std::cout << "Displaying all found Hamiltonian cycles:\n";
    g4.printAllCycles();

    std::cout << "\nFinding maximum edge-disjoint Hamiltonian cycles...\n";
    g4.findMaxEdgeDisjointHamiltonianCycles();

    std::cout << "Displaying maximum edge-disjoint Hamiltonian cycles:\n";
    g4.printMaxEdgeDisjointCycles();

    std::cout << "Test Case 3 Completed\n";
    std::cout << "========================================\n\n";
}

/**
 * Main function
 */
int main() {
    std::cout << "Hamiltonian Cycle Finder Program\n";
    std::cout << "=================================\n\n";

    testCompleteGraphK4();
    testSpecial8VertexGraphCombined();
    test16VertexGraph();

    return 0;
}
