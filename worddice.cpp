// Ethan Dietrich
// William Armentrout
// Project 5: Worddice
// Description: This program takes in "word dice" and determines which of the provided
//          words it is able to spell through the use of a graph and edmondsKarp algorithm.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <climits>
#include <string>
#include <unordered_map>
#include <unordered_set>

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
        adjList[v].push_back(u); // reverse edge for residual graph
    }

    // BFS to find an augmenting path
    bool bfs(int source, int sink, vector<int>& parent) {
        fill(parent.begin(), parent.end(), -1);
        parent[source] = -2; // Mark the source node
        queue<pair<int, int>> q;
        q.push({source, INT_MAX});

        while (!q.empty()) {
            int current = q.front().first;
            int flow = q.front().second;
            q.pop();

            for (int next : adjList[current]) {
                // If there's available capacity and next is not yet visited
                if (parent[next] == -1 && capacity[current][next] > 0) {
                    parent[next] = current;
                    int new_flow = min(flow, capacity[current][next]);
                    if (next == sink) {
                        return true;
                    }
                    q.push({next, new_flow});
                }
            }
        }
        return false;
    }

    // Edmonds-Karp algorithm for finding max flow
    int edmondsKarp(int source, int sink, vector<int>& parent) {
        int flow = 0;
        while (bfs(source, sink, parent)) {
            int path_flow = INT_MAX;
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                path_flow = min(path_flow, capacity[u][v]);
            }

            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                capacity[u][v] -= path_flow;
                capacity[v][u] += path_flow;
            }
            flow += path_flow;
        }
        return flow;
    }
};

// Function to check if a word can be spelled with the given dice and to get the correct dice order
bool canSpellWord(const vector<string>& dice, const string& word, vector<int>& dice_order) {
    
    int num_dice = dice.size();
    int word_len = word.length();
    Graph graph(num_dice + word_len + 2); // +2 for source and sink

    int source = num_dice + word_len;
    int sink = num_dice + word_len + 1;

    // Connect source to each die and each letter to sink
    for (int i = 0; i < num_dice; ++i) {
        graph.addEdge(source, i, 1);
    }
    for (int i = 0; i < word_len; ++i) {
        graph.addEdge(num_dice + i, sink, 1);
    }

    // Connect dice to letters they can spell
    for (int i = 0; i < num_dice; ++i) {
        for (int j = 0; j < word_len; ++j) {
            if (dice[i].find(word[j]) != string::npos) {
                graph.addEdge(i, num_dice + j, 1);
            }
        }
    }

    // Prepare parent vector for tracking flow path and perform max flow calculation
    vector<int> parent(graph.V, -1);
    int max_flow = graph.edmondsKarp(source, sink, parent);
    if (max_flow != word_len) {
        return false;
    }

    // Backtrack to identify exact dice usage in order
    dice_order.clear();
    vector<bool> used_dice(num_dice, false); // Track used dice

    for (int i = 0; i < word_len; ++i) {
        for (int d = 0; d < num_dice; ++d) {
            // Check if die d is used for letter i and hasn't been used before
            if (graph.capacity[num_dice + i][d] > 0 && !used_dice[d]) {
                dice_order.push_back(d);
                used_dice[d] = true; // Mark die as used
                break;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <dice_file> <words_file>" << endl;
        return 1;
    }

    // Get files
    ifstream dice_file(argv[1]); 
    ifstream words_file(argv[2]);
    // Error checking
    if (!dice_file.is_open() || !words_file.is_open()) {
        cerr << "Error opening file(s)." << endl;
        return 1;
    }

    // Read dice
    vector<string> dice;
    string line;
    while (getline(dice_file, line)) {
        dice.push_back(line);
    }

    // Check each word
    while (getline(words_file, line)) {
        string word = line;
        vector<int> dice_order;
        if (canSpellWord(dice, word, dice_order)) {
            for (size_t i = 0; i < dice_order.size(); ++i) {
                cout << dice_order[i];
                if (i < dice_order.size() - 1) cout << ",";
            }
            cout << ": " << word << endl;
        } else {
            cout << "Cannot spell " << word << endl;
        }
    }

    return 0;
}
