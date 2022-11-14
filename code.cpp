#include<bits/stdc++.h>
using namespace std;

#define Max_Capacity 200

int Num_Customers;

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
    vector<int>routes;


    Cost(int num_vehicles, float dist, float fitness,vector<int>routes) {
        this->num_vehicles = num_vehicles;
        this->dist = dist;
        this->fitness = fitness;
        this->routes = routes;
    }

};

void take_input() {
    freopen("input_vrptw.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    for (int i = 0; i < Num_Customers; i++) {
        int cust_id;
        float x_coord, y_coord, demand, ready_time, due_time, service_time;
        cin >> cust_id >> x_coord >> y_coord >> demand >> ready_time >> due_time >> service_time;
        cust_id -= 2;
        Customer cust(cust_id, x_coord, y_coord, demand, ready_time, due_time, service_time);
        customers.push_back(cust);
    }
}
vector<vector<int>> Generate_initial_population() {
    vector<int> init(Num_Customers);
    for (int i = 0; i < Num_Customers; i++) init[i] = i;
    set<vector<int>> st;
    st.insert(init);
    while (st.size() < Num_Customers) {
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
    vector<int>routes;
    int num_vehicles = 1;
    for (int i = 0; i < Num_Customers; i++) {

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
            routes.push_back(i);
            num_vehicles++;
            time = 0;
            demand = 0;
            prev_x = 35;
            prev_y = 35;


            i--;
        }
    }

    float fitness = Num_Customers * num_vehicles + 0.001 * (dist);
    Cost cost(num_vehicles, dist, fitness,routes);

    return cost;
}

void Crossover(vector<vector<int>>population){
    cout<<"population size: "<<population.size()<<endl;
     random_device rd; // obtain a random number from hardware
     mt19937 gen(rd()); // seed the generator
     uniform_int_distribution<> distr(0, population.size()-1);
     //parents
     int father = distr(gen);
     int mother = distr(gen);
     while(father==mother) mother = distr(gen);
     cout<<"random number " <<father<<" "<<mother<<endl;

     Cost x = cost_function(population[father]);
     Cost y = cost_function(population[mother]);
     unordered_map<int,vector<int>>index_map;    // gene -> father index, mother index
     for(int i=0;i<population[father].size();i++){
        index_map[population[father][i]].push_back(i);
     }
     for(int i=0;i<population[mother].size();i++){
        index_map[population[mother][i]].push_back(i);
     }

     vector<int>delete_element[2];

     random_device rnd;
     mt19937 genr(rnd());
     uniform_int_distribution<> distri(0,population[father].size()-1);
     int rf = distr(genr);
     int rm = distr(genr);
     //cout<<"random_device "<<rm<<" " <<rf;

     //for(int i=0;i<)
     

     cout<<"population ";
     for(int i=0;i<population[father].size();i++) cout<<population[father][i]<<" ";
        cout<<endl;
     vector<int>vec = x.routes;
     cout<<"routes ";
     for(int i=0;i<vec.size();i++) cout<<vec[i]<<" ";
        cout<<endl;
    cout<<"num_vehicles "<<x.num_vehicles<<endl;


}

int main() {

    Num_Customers = 25;

    take_input();

    vector<vector<int>> init_pop = Generate_initial_population();


    for (int i = 0; i < Num_Customers; i++) {
        auto  cost = cost_function(init_pop[i]);
        cout << cost.num_vehicles << " " << cost.dist << " " << cost.fitness << endl;
    }
    Crossover(init_pop);

    // for (auto v : init_pop)
    // {
    //     for (auto x : v) {
    //         cout << x << " ";
    //     }
    //     cout << endl;
    // }

}