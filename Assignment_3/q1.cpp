#include <bits/stdc++.h>
using namespace std;

// Define a Router class to represent network routers and perform routing operations
class Router {
private:
    int router_ID;
    vector<pair<Router*, int>> neighbors; // Pair of neighbor router and link weight

public:
    Router(int id) : router_ID(id) {}
    map<int, Router*> routingTable;

    int getRouter_ID() const {
        return router_ID;
    }

    // Add a neighbor router with a given link weight
    void add_neighbor(Router* neighbor, int weight) {
        neighbors.push_back({neighbor, weight});
    }

    // Get the neighbors of this router
    vector<pair<Router*, int>> get_neighbors() const {
        return neighbors;
    }

    // Update the routing table using Dijkstra's Algorithm
    void update_routing_table(set<Router*> allRouters) {
        routingTable.clear();
        map<int, int> distance;
        map<Router*, Router*> nextHop;

        // Initialize distance to infinity and nextHop to NULL for all routers
        for (auto router : allRouters) {
            distance[router->getRouter_ID()] = INT_MAX;
            nextHop[router] = nullptr;
        }

        // Distance to self is 0
        distance[router_ID] = 0;

        // Dijkstra's Algorithm
        priority_queue<pair<int, Router*>> pq;
        pq.push({0, this});

        while (!pq.empty()) {
            Router* node = pq.top().second;
            int dis = pq.top().first;
            pq.pop();

            for (auto& neighbor : node->neighbors) {
                int v = neighbor.first->getRouter_ID();
                int weight = neighbor.second;

                if (distance[node->getRouter_ID()] + weight < distance[v]) {
                    distance[v] = distance[node->getRouter_ID()] + weight;
                    nextHop[neighbor.first] = node;
                    pq.push({-distance[v], neighbor.first});
                }
            }
        }

        // Set next hop to the router itself if no route exists
        for (Router* routerPtr : allRouters) {
            if (nextHop[routerPtr] == nullptr) {
                nextHop[routerPtr] = routerPtr;
            }
        }

        set<Router*> neighborsSet;
        for (auto i : neighbors) {
            neighborsSet.insert(i.first);
        }

        // Build routing table for all routers (including self)
        for (const auto& entry : nextHop) {
            Router* destRouter = entry.first;
            Router* nhRouter = entry.second;

            if (destRouter != nhRouter && nhRouter != this) {
                if (neighborsSet.find(nhRouter) != neighborsSet.end()) {
                    routingTable[destRouter->getRouter_ID()] = nhRouter;
                } else {
                    auto temp = nhRouter;
                    while (neighborsSet.find(temp) == neighborsSet.end()) {
                        temp = nextHop[temp];
                    }
                    routingTable[destRouter->getRouter_ID()] = temp;
                }
            } else if (nhRouter == this) {
                routingTable[destRouter->getRouter_ID()] = destRouter;
            } else {
                routingTable[destRouter->getRouter_ID()] = nullptr;
            }
        }
        for (auto& entry : routingTable) {
            if (neighborsSet.find(entry.second) != neighborsSet.end()) {
                if (routingTable[entry.second->getRouter_ID()] != entry.second) {
                    entry.second = routingTable[entry.second->getRouter_ID()];
                }
            }
        }
    }

    // Find a router by its ID from a set of routers
    Router* getRouterByID(int id, const set<Router*>& allRouters) {
        for (Router* routerPtr : allRouters) {
            if (routerPtr->getRouter_ID() == id) {
                return routerPtr;
            }
        }
        return nullptr;
    }

    // Print the routing table for this router
    void print_routing_table() const {
        cout << "Routing Table for Router " << router_ID << ":\n";
        for (const auto& entry : routingTable) {
            if (entry.first != router_ID) {
                int dest = entry.first;
                Router* nhRouter = entry.second;

                cout << "Destination: " << dest;
                if (nhRouter != nullptr) {
                    cout << " Next Hop: " << nhRouter->getRouter_ID();
                } else {
                    cout << " Path does not exist";
                }
                cout << "\n";
            }
        }
        cout << "-------------------------\n";
    }
};

// Function to find the path between two routers and calculate the least weight
vector<Router*> findPath(int start, int finish, map<int, Router*> routerMap, int& leastWeight) {
    if (start == finish) {
        leastWeight = 0;
        return {routerMap[start]};
    }

    auto startRouter = routerMap[start];
    auto finishRouter = routerMap[finish];
    vector<Router*> path;
    int temp = start;
    leastWeight = 0;

    while (temp != finish) {
        if (routerMap[temp]->routingTable[finish] == nullptr) {
            leastWeight = numeric_limits<int>::max();
            return {};
        } else {
            Router* currentRouter = routerMap[temp];
            Router* nextRouter = routerMap[temp]->routingTable[finish];
            int weight = 0;

            for (const pair<Router*, int>& neighbor : currentRouter->get_neighbors()) {
                if (neighbor.first == nextRouter) {
                    weight = neighbor.second;
                    break;
                }
            }

            path.push_back(currentRouter);
            leastWeight += weight;
            temp = nextRouter->getRouter_ID();
        }
    }
    path.push_back(finishRouter);
    return path;
}

int main() {
    int numRouters, numEdges;

    cout << "Enter the number of routers: ";
    cin >> numRouters;

    // Create routers
    vector<Router> routers;
    for (int i = 1; i <= numRouters; ++i) {
        routers.emplace_back(i);
    }

    // Connect routers by adding neighbors with weights
    cout << "Enter the number of edges: ";
    cin >> numEdges;

    for (int i = 0; i < numEdges; ++i) {
        int start, end, weight;
        cout << "Enter edge (start end weight): ";
        cin >> start >> end >> weight;

        routers[start - 1].add_neighbor(&routers[end - 1], weight);
    }

    set<Router*> allRouters;
    for (Router& router : routers) {
        allRouters.insert(&router);
    }

    // Update and print routing tables for all routers
    for (Router& router : routers) {
        router.update_routing_table(allRouters);
        router.print_routing_table();
    }

    map<int, Router*> numToRouters;
    for (Router& router : routers) {
        numToRouters[router.getRouter_ID()] = &router;
    }

    char continueChoice;
    do {
        int source, destination;
        cout << "Enter source router ID: ";
        cin >> source;
        cout << "Enter destination router ID: ";
        cin >> destination;

        int leastWeight;
        vector<Router*> path = findPath(source, destination, numToRouters, leastWeight);
        if (!path.empty()) {
            cout << "Path from Router " << source << " to Router " << destination << ": ";
            for (size_t i = 0; i < path.size(); ++i) {
                cout << path[i]->getRouter_ID();
                if (i < path.size() - 1) {
                    int weight = 0;
                    for (const pair<Router*, int>& neighbor : path[i]->get_neighbors()) {
                        if (neighbor.first == path[i + 1]) {
                            weight = neighbor.second;
                            break;
                        }
                    }
                    cout << " ---> ";
                }
            }
            cout << " (Routing cost: " << leastWeight << ")" << endl;
        } else {
            cout << "No path from Router " << source << " to Router " << destination << "." << endl;
        }

        cout << "Want to find another path? (Y/N): ";
        cin >> continueChoice;

    } while (continueChoice == 'Y' || continueChoice == 'y');

    return 0;
}
