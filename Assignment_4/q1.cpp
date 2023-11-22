#include <bits/stdc++.h>
using namespace std;

class HttpRequest
{
public:
    int uniqueID;
    int websiteID;
    int processingTime;

    HttpRequest(int id, int webID, int time) : uniqueID(id), websiteID(webID), processingTime(time) {}
};

class Website
{
public:
    int websiteID;
    int ownerID;
    int allocatedBandwidth;
    int allocatedProcessingPower;
    queue<HttpRequest> requestQueue; // Store request IDs for printing

    Website() = default; // Default constructor

    Website(int webID, int owner, int bandwidth, int power) : websiteID(webID), ownerID(owner), allocatedBandwidth(bandwidth), allocatedProcessingPower(power) {}
};

class LoadBalancer
{
public:
    unordered_map<int, Website> websites;
    unordered_map<int, int> resources;
    unordered_map<int, double> weights;
    unordered_map<int, double> finish_times;
    set<int> websiteIDs;

    set<int> usedRequestIDs; // To track used request IDs

    void updateWeights()
    {
        if (websites.size() == 0)
            return;
        int totalAllocatedResources = 0;
        for (auto id : websiteIDs)
        {
            totalAllocatedResources += resources[id];
        }
        double min_weight = DBL_MAX;
        for (auto id : websiteIDs)
        {
            weights[id] = resources[id] * 1.0 / totalAllocatedResources * 1.0;
            if (weights[id] < min_weight)
                min_weight = weights[id];
        }
        for (auto id : websiteIDs)
        {
            weights[id] = weights[id] / min_weight;
        }
    }
    void add_website(int websiteID, int ownerID, int allocatedBandwidth, int allocatedProcessingPower)
    {
        if (websites.find(websiteID) != websites.end())
        {
            cout << "Website id already in use." << endl;
            return;
        }
        websites.emplace(websiteID, Website(websiteID, ownerID, allocatedBandwidth, allocatedProcessingPower));
        websiteIDs.insert(websiteID);
        resources.emplace(websiteID, allocatedBandwidth * allocatedProcessingPower);
        finish_times.emplace(websiteID, 0.0);
        updateWeights();
    }

    void enqueue_request(HttpRequest httpRequest)
    {
        if (usedRequestIDs.find(httpRequest.uniqueID) != usedRequestIDs.end())
        {
            cout << "Request ID " << httpRequest.uniqueID << " is already taken. Please choose a different one." << endl;
            return;
        }
        if (websiteIDs.find(httpRequest.websiteID) == websiteIDs.end())
        {
            cout << "Invalid website ID." << endl;
            return;
        }

        websites[httpRequest.websiteID].requestQueue.push(httpRequest);
        usedRequestIDs.insert(httpRequest.uniqueID);
    }
    void dequeue_request()
    {
        double finish_time = DBL_MAX;
        int wfq_wid = -1;
        for (auto id : websiteIDs)
        {
            if (!websites[id].requestQueue.empty())
            {
                if (finish_time > finish_times[id] + websites[id].requestQueue.front().processingTime * 1.0 / weights[id])
                {
                    finish_time = finish_times[id] + websites[id].requestQueue.front().processingTime * 1.0 / weights[id];
                    wfq_wid = id;
                }
                else if (finish_time == finish_times[id] + websites[id].requestQueue.front().processingTime * 1.0 / weights[id])
                {
                    if (weights[id] > weights[wfq_wid])
                        wfq_wid = id;
                }
            }
        }
        if (wfq_wid == -1)
        {
            cout << "No requests to dequeue." << endl;
            return;
        }
        cout << "Request ID " << websites[wfq_wid].requestQueue.front().uniqueID << " dequeued from website " << websites[wfq_wid].requestQueue.front().websiteID << endl;
        finish_times[wfq_wid] = finish_time;
        websites[wfq_wid].requestQueue.pop();
    }
};
void printHttpRequests(const Website &website)
{

    queue<HttpRequest> tempQueue = website.requestQueue;

    while (!tempQueue.empty())
    {
        const HttpRequest &request = tempQueue.front();
        cout << "ID: " << request.uniqueID << " Time: " << request.processingTime << " | ";
        tempQueue.pop();
    }
}
int main()
{
    LoadBalancer loadBalancer;

    while (true)
    {
        // Print website information, including weights and corresponding HTTP request IDs and processing times

        if (loadBalancer.weights.size() != 0)
        {
            cout << "\n";
            for (auto it : loadBalancer.websiteIDs)
            {
                cout << "Website " << it << " (Weight: " << loadBalancer.weights[it] << ") | ";
                printHttpRequests(loadBalancer.websites[it]);
                cout << endl;
            }
        }

        cout << "Choose one:" << endl;
        cout << "1. Add Website" << endl;
        cout << "2. Enqueue HTTP Request" << endl;
        cout << "3. Dequeue HTTP Request" << endl;
        cout << "4. Exit" << endl;

        int choice;
        cout << "Enter process: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            int websiteID, ownerID, allocatedBandwidth, allocatedProcessingPower;
            cout << "Enter Website ID, Owner ID, Allocated Bandwidth, Allocated Processing Power: ";
            cin >> websiteID >> ownerID >> allocatedBandwidth >> allocatedProcessingPower;
            loadBalancer.add_website(websiteID, ownerID, allocatedBandwidth, allocatedProcessingPower);
            break;
        }
        case 2:
        {
            int uniqueID, websiteID, processingTime;
            cout << "Enter Request ID, Website ID, Processing Time: ";
            cin >> uniqueID >> websiteID >> processingTime;

            loadBalancer.enqueue_request(HttpRequest(uniqueID, websiteID, processingTime));

            break;
        }
        case 3:
            loadBalancer.dequeue_request();
            break;
        case 4:
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}