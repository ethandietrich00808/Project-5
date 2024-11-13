// Ethan Dietrich
// Project 5: Worddice

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <climits>
#include <string>

using namespace std;

// Class to represent a graph for the flow network
class Graph {
public:
    int V; // Number of vertices
    vector<vector<int>> capacity; // Capacity matrix
    vector<vector<int>> adjList; // Adjacency list

    // Constructor
    Graph(int V) : V(V), capacity(V, vector<int>(V, 0)), adjList(V) {}

    // Add an edge from u to v with given capacity
    void addEdge(int u, int v, int cap) {
        capacity[u][v] += cap;
        adjList[u].push_back(v);
        adjList[v].push_back(u); // reverse edge
    }

    // BFS to find an augmenting path
    bool bfs(int source, int sink, vector<int>& parent) {
        fill(parent.begin(), parent.end(), -1);
        parent[source] = -2; // Mark the source node
        queue<pair<int, int>> q;
        q.push({source, INT_MAX});

        while (!q.empty()) {
            int node = q.front().first;
            int flow = q.front().second;
            q.pop();

            for (int neighbor : adjList[node]) {
                if (parent[neighbor] == -1 && capacity[node][neighbor] > 0) {
                    parent[neighbor] = node;
                    int new_flow = min(flow, capacity[node][neighbor]);
                    if (neighbor == sink) return true;
                    q.push({neighbor, new_flow});
                }
            }
        }
        return false;
    }

    // Edmonds-Karp algorithm for finding the maximum flow
    int edmondsKarp(int source, int sink) {
        int flow = 0;
        vector<int> parent(V);

        // Augment the flow while there is an augmenting path
        while (bfs(source, sink, parent)) {
            int current_flow = INT_MAX;
            int s = sink;
            while (s != source) {
                int p = parent[s];
                current_flow = min(current_flow, capacity[p][s]);
                s = p;
            }

            flow += current_flow;
            s = sink;
            while (s != source) {
                int p = parent[s];
                capacity[p][s] -= current_flow;
                capacity[s][p] += current_flow;
                s = p;
            }
        }
        return flow;
    }
};

// Function to read the dice and words from the files and set up the graph
void readorig(const string& diceFile, const string& wordsFile, Graph& graph, vector<string>& dice, vector<string>& words) {
    ifstream diceStream(diceFile);
    ifstream wordsStream(wordsFile);
    string line;

    // Read dice from file
    while (getline(diceStream, line)) {
        dice.push_back(line);
    }

    // Read words from file
    while (getline(wordsStream, line)) {
        words.push_back(line);
    }

    // Create graph edges: source -> dice nodes, dice nodes -> word nodes, word nodes -> sink
    int nodeOffset = dice.size();
    int source = 0;
    int sink = nodeOffset + words.size() + 1;

    // Create the edges from source to dice nodes
    for (int i = 0; i < dice.size(); ++i) {
        graph.addEdge(source, i + 1, 1); // Source to dice nodes
    }

    // Create edges from dice nodes to word nodes
    for (int i = 0; i < dice.size(); ++i) {
        for (int j = 0; j < words.size(); ++j) {
            for (char c : words[j]) {
                if (dice[i].find(c) != string::npos) {
                    graph.addEdge(i + 1, nodeOffset + j + 1, 1); // Dice to words
                    break;
                }
            }
        }
    }

    // Create edges from word nodes to sink
    for (int i = 0; i < words.size(); ++i) {
        graph.addEdge(nodeOffset + i + 1, sink, 1); // Word nodes to sink
    }
}

// Function to solve the worddice problem
void solveWordDice(const vector<string>& dice, const vector<string>& words, Graph& graph) {
    int nodeOffset = dice.size();
    int source = 0;
    int sink = nodeOffset + words.size() + 1;

    // Calculate maximum flow from source to sink
    int maxFlow = graph.edmondsKarp(source, sink);

    // Check each word to see if it can be spelled
    for (const string& word : words) {
        vector<int> parent(graph.V);
        if (graph.bfs(source, sink, parent)) {
            bool canSpell = true;
            for (char c : word) {
                bool found = false;
                for (int i = 0; i < dice.size(); ++i) {
                    if (dice[i].find(c) != string::npos) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    canSpell = false;
                    break;
                }
            }

            // Output if the word can be spelled
            if (canSpell) {
                cout << word << " can be spelled." << endl;
            } else {
                cout << "Cannot spell " << word << endl;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: worddice <diceFile> <wordsFile>" << endl;
        return 1;
    }

    string diceFile = argv[1];
    string wordsFile = argv[2];

    vector<string> dice;
    vector<string> words;

    // Create graph with appropriate size (source + sink + dice + words)
    Graph graph(dice.size() + words.size() + 2);

    // Read dice and words from file and build graph
    readorig(diceFile, wordsFile, graph, dice, words);

    // Solve the problem using the graph and the flow algorithm
    solveWordDice(dice, words, graph);

    return 0;
}