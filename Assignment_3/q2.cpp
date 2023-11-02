#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

class Node {
public:
    Node(int id) : id(id), backoff(0), successfulTransmissions(0), collisions(0), backoffCount(0) {}

    void transmit(int currentTime, int contentionWindow) {
        if (backoff == 0) {
            cout << "Node " << id << " is transmitting at time " << currentTime << endl;
            successfulTransmissions++;
            backoffCount = 0;
            backoff = rand() % contentionWindow; // Reset backoff
        } else {
            cout << "Node " << id << " is backing off at time " << currentTime << " with backoff " << backoff << endl;
            backoff--;
            backoffCount++;
        }
    }

    void startBackoff(int contentionWindow) {
        backoff = rand() % contentionWindow;
    }

    int getId() {
        return id;
    }

    int getSuccessfulTransmissions() {
        return successfulTransmissions;
    }

    int getCollisions() {
        return collisions;
    }

    int getBackoffCount() {
        return backoffCount;
    }

    void incrementCollisions() {
        collisions++;
    }

    int backoff; // Move 'backoff' to the public section

private:
    int id;
    int successfulTransmissions;
    int collisions;
    int backoffCount;
};

int main() {
    const int N = 5; // Number of nodes
    const int contentionWindow = 10; // Adjustable backoff window

    vector<Node> network;

    srand(time(nullptr)); // Seed the random number generator

    for (int i = 0; i < N; i++) {
        network.push_back(Node(i));
    }

    int currentTime = 0;
    int simulationTime = 100; // Total simulation time
	int c = 0;

	for (int i = 0; i < N; i++) {
		network[i].transmit(currentTime, contentionWindow);
	}


	cout << endl;

	cout << "Collision at time " << currentTime << endl << endl;

	for (int i = 0; i < N; i++){
		network[i].incrementCollisions();
	}

		// Simulate backoff mechanism
		for (int i = 0; i < N; i++)
		{
			if (network[i].backoff == 0)
			{
				// Node is eligible for a new transmission attempt, start a new backoff
				network[i].startBackoff(contentionWindow);
			}
		}

	currentTime++;

	while (currentTime < simulationTime) {
        // Simulate nodes attempting to transmit
		for (int i = 0; i < N; i++) {
            network[i].transmit(currentTime, contentionWindow);
        }

        cout << endl;
        // Simulate backoff mechanism
        // for (int i = 0; i < N; i++) {
        //     if (network[i].backoff == 0) {
        //         // Node is eligible for a new transmission attempt, start a new backoff
        //         network[i].startBackoff(contentionWindow);
        //     }
        // }

        // Check for collisions
        int busyChannels = 0;
        for (int i = 0; i < N; i++) {
            if (network[i].backoff == 0) {
                busyChannels++;
            }
        }

        if (busyChannels > 1) {
            // Collision occurred
            cout << "Collision at time " << currentTime << endl << endl;
            for (int i = 0; i < N; i++) {
                if (network[i].backoff == 0) {
                    network[i].incrementCollisions();
                    network[i].backoff = rand() % contentionWindow;
                }
            }
        }

		else{
			
		}

        currentTime++;
    }

    // Print statistics
    for (int i = 0; i < N; i++) {
        cout << "Node " << i << " - Successful Transmissions: " << network[i].getSuccessfulTransmissions() << ", Collisions: " << network[i].getCollisions() << ", Backoff Count: " << network[i].getBackoffCount() << endl;
    }

    return 0;
}