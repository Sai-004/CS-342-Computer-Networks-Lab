#include <bits/stdc++.h>
using namespace std;

// Function to generate exponential random variable with rate lambda
double generateExponentialTime(double lambda)
{
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<double> exp_dist(lambda);
    return exp_dist(gen);
}

// Class representing a passenger
class Passenger
{
public:
    int arrival;
    int service_time_left;
    int serviced_time;

    Passenger(int arr)
    {
        arrival = arr;
        service_time_left = -1;
        serviced_time = 0;
    }
};

// Class representing a server
class server
{
public:
    int busy;
    int serving_time;
    int temp_serving;

    server()
    {
        busy = 0;
        serving_time = 0;
        temp_serving = -1;
    }
};

// Class representing a security system
class security
{
public:
    int total_passengers;
    int buffer_size;
    vector<server *> all_servers;
    deque<Passenger *> wait_queue;

    // Constructor to initialize the security system
    security(int k, int number_of_servers)
    {
        if (k == -1)
        {
            buffer_size = INT_MAX;
        }
        else
        {
            buffer_size = k;
        }
        total_passengers = 0;
        all_servers.resize(number_of_servers);
        for (int i = 0; i < number_of_servers; i++)
        {
            all_servers[i] = new server();
        }
    }
};

int main()
{
    // Input arrival and departure rates
    double arrival_rate, dep_rate;
    cout << "Enter arrival rate:";
    cin >> arrival_rate;
    cout << "Enter Departure rate:";
    cin >> dep_rate;

    // Variables for simulation
    long long simulated_time;
    long long tot_passengers = 0;
    long long tot_waiting_time = 0;
    long long current_time;
    long long next_arrival_time;
    long long total;

    // Simulation loop for a single server without buffer
    while (1)
    {
        // Input simulation time
        cout << "Enter the simulated time:";
        cin >> simulated_time;

        // Initialize a security system with one server and no buffer
        security security1(-1, 1);
        current_time = 0;
        next_arrival_time = generateExponentialTime(arrival_rate);
        total = 0;

        // Simulation loop for the specified time
        for (current_time = 0; current_time < simulated_time; current_time++)
        {
            // Passenger arrival
            if (current_time == next_arrival_time)
            {
                Passenger *new_passenger = new Passenger(current_time);
                security1.wait_queue.push_back(new_passenger);
                total++;

                // Generate next arrival time
                while ((next_arrival_time = generateExponentialTime(arrival_rate)) == 0)
                {
                    Passenger *new_passenger = new Passenger(current_time);
                    total++;
                    security1.wait_queue.push_back(new_passenger);
                }
                next_arrival_time += current_time;
            }

            // Check if server is idle and there are passengers in the queue
            if ((security1.all_servers[0]->busy == 0 || security1.all_servers[0]->temp_serving == 0) && security1.wait_queue.size() != 0)
            {
                int temp;

                // Service passengers with exponential service time
                while ((temp = (generateExponentialTime(dep_rate))) == 0 && security1.wait_queue.size() != 0)
                {
                    Passenger *to_be_served = security1.wait_queue.front();
                    security1.wait_queue.pop_front();
                    tot_waiting_time += current_time - to_be_served->arrival;
                }

                // If there are passengers in the queue, serve the next one
                if (security1.wait_queue.size() == 0)
                {
                    continue;
                }
                else
                {
                    Passenger *to_be_served = security1.wait_queue.front();
                    security1.wait_queue.pop_front();
                    tot_waiting_time += current_time - to_be_served->arrival;
                    security1.all_servers[0]->busy = 1;
                    security1.all_servers[0]->temp_serving = temp;
                }
            }

            // If server is currently serving a passenger, decrement service time
            if (security1.all_servers[0]->temp_serving > 0)
            {
                security1.all_servers[0]->temp_serving--;
                security1.all_servers[0]->serving_time++;
            }

            // Update total number of passengers in the queue
            tot_passengers += security1.wait_queue.size();
        }

        // Output simulation results for a single server without buffer
        cout << "Avg queue length in single server:" << (double)tot_passengers / simulated_time << endl;
        cout << "Avg waiting time in single server:" << (double)tot_waiting_time / total << endl;
        cout << "Server utilization in single server:" << (double)security1.all_servers[0]->serving_time / simulated_time << endl;

        // Prompt user to rerun or exit
        char req;
        cout << "Note: we took int in place of double" << endl;
        cout << "Do you want to rerun?(y/n):";
        cin >> req;
        if (req == 'n')
            break;
    }

    // Simulation loop for a single server with buffer
    while (1)
    {
        // Input buffer size and simulation time
        cout << "Enter buffer size:";
        int buf_size;
        cin >> buf_size;
        cout << "Enter the simulated time:";
        cin >> simulated_time;
        tot_passengers = 0;
        tot_waiting_time = 0;

        // Initialize a security system with one server and a buffer
        security security2(buf_size, 1);
        current_time = 0;
        next_arrival_time = generateExponentialTime(arrival_rate);
        total = 0;

        // Simulation loop for the specified time
        for (current_time = 0; current_time < simulated_time; current_time++)
        {
            // Passenger arrival if there is space in the buffer
            if (current_time == next_arrival_time && security2.wait_queue.size() < buf_size)
            {
                Passenger *new_passenger = new Passenger(current_time);
                security2.wait_queue.push_back(new_passenger);
                total++;

                // Generate next arrival time
                while ((next_arrival_time = (generateExponentialTime(arrival_rate))) == 0 && security2.wait_queue.size() < buf_size)
                {
                    Passenger *new_passenger = new Passenger(current_time);
                    security2.wait_queue.push_back(new_passenger);
                    total++;
                }
                next_arrival_time += current_time;
            }

            // Check if server is idle and there are passengers in the queue
            if ((security2.all_servers[0]->busy == 0 || security2.all_servers[0]->temp_serving == 0) && security2.wait_queue.size() != 0)
            {
                int temp;

                // Service passengers with exponential service time
                while ((temp = (generateExponentialTime(dep_rate))) == 0 && security2.wait_queue.size() != 0)
                {
                    Passenger *to_be_served = security2.wait_queue.front();
                    security2.wait_queue.pop_front();
                    tot_waiting_time += current_time - to_be_served->arrival;
                }

                // If there are passengers in the queue, serve the next one
                if (security2.wait_queue.size() == 0)
                {
                    continue;
                }
                else
                {
                    Passenger *to_be_served = security2.wait_queue.front();
                    security2.wait_queue.pop_front();
                    tot_waiting_time += current_time - to_be_served->arrival;
                    security2.all_servers[0]->busy = 1;
                    security2.all_servers[0]->temp_serving = temp;
                }
            }

            // If server is currently serving a passenger, decrement service time
            if (security2.all_servers[0]->temp_serving > 0)
            {
                security2.all_servers[0]->temp_serving--;
                security2.all_servers[0]->serving_time++;
            }

            // Update total number of passengers in the queue
            tot_passengers += security2.wait_queue.size();
        }

        // Output simulation results for a single server with buffer
        cout << "Avg queue length in single server:" << (double)tot_passengers / simulated_time << endl;
        cout << "Avg waiting time in single server:" << (double)tot_waiting_time / total << endl;
        cout << "Server utilization in single server with finite buffer of size " << buf_size << ":" << (double)security2.all_servers[0]->serving_time / simulated_time << endl;

        // Prompt user to change buffer size or exit
        char req;
        cout << "Do you want to change the buffer size?(y/n):";
        cin >> req;
        if (req == 'n')
            break;
    }

    // Simulation loop for multiple servers without buffer
    while (1)
    {
        // Input simulation time and number of servers
        cout << "Enter the simulated time:";
        cin >> simulated_time;
        tot_passengers = 0;
        tot_waiting_time = 0;
        int num_servers;
        cout << "Enter the number of servers:";
        cin >> num_servers;

        // Initialize a security system with multiple servers and no buffer
        security security3(-1, num_servers);
        current_time = 0;
        next_arrival_time = generateExponentialTime(arrival_rate);
        int tot_serving_time = 0;
        int tot_serving_time2 = 0;
        total = 0;

        // Simulation loop for the specified time
        for (current_time = 0; current_time < simulated_time; current_time++)
        {
            // Passenger arrival
            if (current_time == next_arrival_time)
            {
                Passenger *new_passenger = new Passenger(current_time);
                security3.wait_queue.push_back(new_passenger);
                total++;

                // Generate next arrival time
                while ((next_arrival_time = (generateExponentialTime(arrival_rate))) == 0)
                {
                    Passenger *new_passenger = new Passenger(current_time);
                    security3.wait_queue.push_back(new_passenger);
                    total++;
                }
                next_arrival_time += current_time;
            }

            // Service passengers for each server
            for (int i = 0; i < num_servers; i++)
            {
                if ((security3.all_servers[i]->busy == 0 || security3.all_servers[i]->temp_serving == 0) && security3.wait_queue.size() != 0)
                {
                    int temp;

                    // Service passengers with exponential service time
                    while ((temp = (generateExponentialTime(dep_rate))) == 0 && security3.wait_queue.size() != 0)
                    {
                        Passenger *to_be_served = security3.wait_queue.front();
                        security3.wait_queue.pop_front();
                        tot_waiting_time += current_time - to_be_served->arrival;
                    }

                    // If there are passengers in the queue, serve the next one
                    if (security3.wait_queue.size() == 0)
                    {
                        continue;
                    }
                    else
                    {
                        Passenger *to_be_served = security3.wait_queue.front();
                        security3.wait_queue.pop_front();
                        tot_waiting_time += current_time - to_be_served->arrival;
                        security3.all_servers[i]->busy = 1;
                        security3.all_servers[i]->temp_serving = temp;
                    }
                }
            }

            // Decrement service time for each server
            int served = 0;
            bool served2 = false;
            for (int i = 0; i < num_servers; i++)
            {
                if (security3.all_servers[i]->temp_serving > 0)
                {
                    if (served == 0)
                        served = 1;
                    served2 = true;
                    security3.all_servers[i]->temp_serving--;
                    security3.all_servers[i]->serving_time++;
                }
                else
                {
                    served = 2;
                }
                if (security3.all_servers[i]->temp_serving == 0)
                {
                    security3.all_servers[i]->busy = 0;
                }
            }

            // Update total number of passengers in the queue
            if (served == 1)
            {
                tot_serving_time++;
            }
            if (served2 == true)
            {
                tot_serving_time2++;
            }
            tot_passengers += security3.wait_queue.size();
        }

        // Output simulation results for multiple servers without buffer
        cout << "Avg queue length in multi server:" << (double)tot_passengers / simulated_time << endl;
        cout << "Avg waiting time in multi server:" << (double)tot_waiting_time / (total) << endl;
        cout << "Server utilization in multi server:" << (double)(tot_serving_time + tot_serving_time2) / (2 * simulated_time) << endl;

        // Prompt user to enter the number of servers again or exit
        char req;
        cout << "Do you want to enter the number of servers again?(y/n):";
        cin >> req;
        if (req == 'n')
            break;
    }

    // Simulation loop for multiple servers with buffer
    while (1)
    {
        // Input simulation time, number of servers, and buffer size
        cout << "Enter the simulated time:";
        cin >> simulated_time;
        tot_passengers = 0;
        tot_waiting_time = 0;
        int num_servers;
        cout << "Enter the number of servers:";
        cin >> num_servers;
        security security4(-1, num_servers);
        current_time = 0;
        next_arrival_time = (generateExponentialTime(arrival_rate));
        int buf_size;
        cout << "Enter the buffer size:";
        cin >> buf_size;
        int tot_serving_time = 0;
        int tot_serving_time2 = 0;
        total = 0;

        // Simulation loop for the specified time
        for (current_time = 0; current_time < simulated_time; current_time++)
        {
            // Passenger arrival if there is space in the buffer
            if (current_time == next_arrival_time && security4.wait_queue.size() < buf_size)
            {
                Passenger *new_passenger = new Passenger(current_time);
                security4.wait_queue.push_back(new_passenger);
                total++;
                while ((next_arrival_time = (generateExponentialTime(arrival_rate))) == 0 && security4.wait_queue.size() < buf_size)
                {
                    Passenger *new_passenger = new Passenger(current_time);
                    security4.wait_queue.push_back(new_passenger);
                    total++;
                }
                next_arrival_time += current_time;
            }
            for (int i = 0; i < num_servers; i++)
            {
                if ((security4.all_servers[i]->busy == 0 || security4.all_servers[i]->temp_serving == 0) && security4.wait_queue.size() != 0)
                {
                    int temp;
                    while ((temp = (generateExponentialTime(dep_rate))) == 0 && security4.wait_queue.size() != 0)
                    {
                        Passenger *to_be_served = security4.wait_queue.front();
                        security4.wait_queue.pop_front();
                        tot_waiting_time += current_time - to_be_served->arrival;
                    }
                    if (security4.wait_queue.size() == 0)
                    {
                        continue;
                    }
                    else
                    {
                        Passenger *to_be_served = security4.wait_queue.front();
                        security4.wait_queue.pop_front();
                        tot_waiting_time += current_time - to_be_served->arrival;
                        security4.all_servers[i]->busy = 1;
                        security4.all_servers[i]->temp_serving = temp;
                    }
                }
            }
            int served = 0;
            bool served2 = false;
            for (int i = 0; i < num_servers; i++)
            {
                if (security4.all_servers[i]->temp_serving > 0)
                {
                    if (served == 0)
                        served = 1;
                    served2 = true;
                    security4.all_servers[i]->temp_serving--;
                    security4.all_servers[i]->serving_time++;
                }
                else
                {
                    served = 2;
                }
                if (security4.all_servers[i]->temp_serving == 0)
                {
                    security4.all_servers[i]->busy = 0;
                }
            }
            if (served == 1)
            {
                tot_serving_time++;
            }
            if (served2 == true)
            {
                tot_serving_time2++;
            }
            tot_passengers += security4.wait_queue.size();
        }
        cout << "Avg queue length in multi server with buffer size " << buf_size << ":" << (double)tot_passengers / simulated_time << endl;
        // cout<<"total waiting time:"<<tot_waiting_time<<endl;
        cout << "Avg waiting time in multi server with buffer size " << buf_size << ":" << (double)tot_waiting_time / total << endl;
        cout << "Server utilization in multi server with buffer size " << buf_size << ":" << (double)(tot_serving_time + tot_serving_time2) / (2 * simulated_time) << endl;
        char req;
        cout << "Do you want to enter number of servers and buffer size again?(y/n):";
        cin >> req;
        if (req == 'n')
            break;
    }
}
