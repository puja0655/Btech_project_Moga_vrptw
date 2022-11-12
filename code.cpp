#include<bits/stdc++.h>
using namespace std;

#define Max_Capacity 200



class Customer {
public:
    int cust_id;
    float x_coord, y_coord, demand, ready_time, due_time, service_time;


    Customer(int cust_id, float x_coord, float y_coord, float demand, float ready_time, float due_time, float service_time) {
        this->cust_id = cust_id;
        this->x_coord = x_coord;
        this->y_coord = y_coord;
        this->demand = demand;
        this->ready_time = ready_time;
        this->due_time = due_time;
        this->service_time = service_time;

    }
    void print_data() {
        cout << this->cust_id << " " << this->x_coord << " " << this->y_coord << " " << this->demand << " " << this->ready_time << " " << this->due_time << " " << this->service_time << endl ;
    }
};

vector<Customer> customers;

class Cost {

public:
    int num_vehicles;
    float dist;
    float fitness;


    Cost(int num_vehicles, float dist, float fitness) {
        this->num_vehicles = num_vehicles;
        this->dist = dist;
        this->fitness = fitness;
    }

};

void take_input() {
    freopen("input_vrptw.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    for (int i = 0; i < 100; i++) {
        int cust_id;
        float x_coord, y_coord, demand, ready_time, due_time, service_time;
        cin >> cust_id >> x_coord >> y_coord >> demand >> ready_time >> due_time >> service_time;
        cust_id -= 2;
        Customer cust(cust_id, x_coord, y_coord, demand, ready_time, due_time, service_time);
        customers.push_back(cust);
    }
}
vector<vector<int>> Generate_initial_population() {
    vector<int> init(100);
    for (int i = 0; i < 100; i++) init[i] = i;
    set<vector<int>> st;
    st.insert(init);
    while (st.size() < 100) {
        random_shuffle(init.begin(), init.end());
        st.insert(init);
    }
    vector<vector<int>> init_pop;
    for (auto v : st)
    {
        init_pop.push_back(v);
    }
    random_shuffle(init_pop.begin(), init_pop.end());
    return init_pop;
}

Cost cost_function(vector<int> chromosome) {
    float prev_x = 35, prev_y = 35;
    float time = 0, dist = 0, demand = 0;

    int num_vehicles = 1;
    for (int i = 0; i < 100; i++) {

        Customer gene = customers[chromosome[i]];
        if (demand + gene.demand <= Max_Capacity and (time + gene.service_time) <= gene.due_time) {

            if (time < gene.ready_time) {
                time = gene.ready_time;
            }
            float x = gene.x_coord;
            float y = gene.y_coord;

            float dist_bw = sqrt((prev_x - x) * (prev_x - x) + (prev_y - y) * (prev_y - y));
            dist += dist_bw;

            demand += gene.demand;

            time += gene.service_time;

            prev_x = x;
            prev_y = y;
        }
        else {

            // going back to warehouse
            num_vehicles++;
            time = 0;
            demand = 0;
            prev_x = 35;
            prev_y = 35;


            i--;
        }
    }

    float fitness = 100 * num_vehicles + 0.001 * (dist);
    Cost cost(num_vehicles, dist, fitness);

    return cost;
}

int main() {

    take_input();

    vector<vector<int>> init_pop = Generate_initial_population();


    for (int i = 0; i < 100; i++) {
        auto  cost = cost_function(init_pop[i]);
        cout << cost.num_vehicles << " " << cost.dist << " " << cost.fitness << endl;
    }

    // for (auto v : init_pop)
    // {
    //     for (auto x : v) {
    //         cout << x << " ";
    //     }
    //     cout << endl;
    // }

}