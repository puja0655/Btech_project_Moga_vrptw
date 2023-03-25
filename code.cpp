#include<bits/stdc++.h>
#include <cstdlib>
#include <ctime>

using namespace std;

#define Max_Capacity 200

int Num_Customers;
int Num_Chromosomes;
int alpha;
int beta;

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


    Cost(int num_vehicles, float dist, float fitness, vector<int>routes) {
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
vector<int> random_vec(int range, int len) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int r;
    // r = std::rand();
    // r = r % 100;
    // return r;
    unordered_set<int> st;
    while (st.size() < len) {
        r = std::rand();
        r = r % range;
        st.insert(r);
    }
    vector<int> randoms;
    for (auto x : st) {
        randoms.push_back(x);
        //cout<<x<<endl;
    }
    return randoms;
}


vector<vector<int>> Generate_initial_population() {
    vector<int> init(Num_Customers);
    for (int i = 0; i < Num_Customers; i++) init[i] = i;
    set<vector<int>> st;
    st.insert(init);
    while (st.size() < Num_Chromosomes) {
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

vector<int> pareto_ranking(vector<pair<int, float>> costs) {
    vector<int> rank(costs.size(), INT_MAX);
    int r = 1;
    while (1) {
        bool all_done = true;
        for (int i = 0; i < costs.size(); i++) {
            if (rank[i] == INT_MAX) {
                all_done = false;
                bool flag = true;
                // checking if i is non-dominated
                for (int j = 0; j < costs.size(); j++) {
                    if (rank[j] == INT_MAX) {
                        if (costs[j].first <= costs[i].first and costs[j].second <= costs[i].second) {
                            if (costs[j].first < costs[i].first or costs[j].second < costs[i].second) {
                                flag = false;
                                break;
                            }
                        }
                    }
                }
                if (flag) rank[i] = r;
            }

        }
        if (all_done) break;
        r++;
    }

    return rank;

}

Cost cost_function(vector<int> chromosome) {
    float prev_x = 35, prev_y = 35;
    float time = 0, dist = 0, demand = 0;
    vector<int>routes;
    int num_vehicles = 1;
    for (int i = 0; i < chromosome.size(); i++) {

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

            // adding dist of last customer in previous route to origin
            i--;

            gene = customers[chromosome[i]];
            float x = gene.x_coord;
            float y = gene.y_coord;

            float dist_bw = sqrt((prev_x - x) * (prev_x - x) + (prev_y - y) * (prev_y - y));
            dist += dist_bw;
        }
    }

    // adding dist of last customer to origin
    prev_x = 35;
    prev_y = 35;
    Customer gene = customers[chromosome[chromosome.size() - 1]];
    float x = gene.x_coord;
    float y = gene.y_coord;

    float dist_bw = sqrt((prev_x - x) * (prev_x - x) + (prev_y - y) * (prev_y - y));
    dist += dist_bw;

    float fitness = alpha * num_vehicles + beta * (dist);
    Cost cost(num_vehicles, dist, fitness, routes);

    return cost;
}

vector<int> Mutation_util(vector<int> chromosome, int rand_index) {
    // random_device rd; // obtain a random number from hardware
    // mt19937 gen(rd()); // seed the generator
    // uniform_int_distribution<> distr(0, chromosome.size() - 2);
    // //parents

    // int rand_index = distr(gen);
    //cout << "rand_index " << rand_index << endl;
    auto child = chromosome;

    swap(child[rand_index], child[rand_index + 1]);
    return child;
}

void Mutation(vector<vector<int>> &population) {

    // random_device rd; // obtain a random number from hardware
    // mt19937 gen(rd()); // seed the generator
    // uniform_int_distribution<> distr(0, population.size() - 1);


    int pop_10 = population.size() / 10;
    vector<int> rand_pop = random_vec(population.size() - 1, pop_10);
    vector<int> rand_indexes = random_vec(population[0].size() - 2, pop_10);
    for (int i = 0; i < pop_10; i++) {
        auto child = Mutation_util(population[rand_pop[i]], rand_indexes[i]);
        population.push_back(child);
    }
}

//utility function to insert deleted genes in best position possible
vector<vector<int>> insert_util(vector<int>father, vector<int>mother, vector<int>delete_element[], int route_length_father, int route_length_mother) {
    vector<int>child1, child2; //childrens generated after crossover
    // cout << "hi9" << endl;
    //insert deleted element in father
    for (int j = 0; j < delete_element[0].size(); j++) {
        // cout << "hi10" << endl;
        int fit = INT_MAX;
        for (int i = 0; i < father.size(); i++) {
            vector<int>c1;
            c1 = father;
            c1.insert(c1.begin() + i, delete_element[0][j]);
            Cost c1_cost = cost_function(c1);
            //number of routes should not increase
            if ((c1_cost.routes).size() <= route_length_father && c1_cost.fitness <= fit) {
                child1 = c1;
                fit = c1_cost.fitness;
            }


        }
        // cout << "hi11" << endl;
        vector<int>cl = father;
        cl.push_back(delete_element[0][j]);
        Cost cl_cost = cost_function(cl);
        if ((cl_cost.routes).size() <= route_length_father && cl_cost.fitness <= fit) {
            child1 = cl;
            fit = cl_cost.fitness;
        }
        // cout << "hi125" << endl;
        if (child1.size() == 0) child1 = cl; //if routes is increasing irrespective of position
        father = child1;
    }
    //cout << "hi12" << endl;
    //insert deleted element in mother
    for (int j = 0; j < delete_element[1].size(); j++) {
        int fit = INT_MAX;
        for (int i = 0; i < mother.size(); i++) {
            vector<int>c1;
            c1 = mother;
            c1.insert(c1.begin() + i, delete_element[1][j]);
            Cost c1_cost = cost_function(c1);
            if ((c1_cost.routes).size() <= route_length_mother && c1_cost.fitness < fit) {
                child2 = c1;
                fit = c1_cost.fitness;
            }

        }
        vector<int>cl = mother;
        cl.push_back(delete_element[1][j]);
        Cost cl_cost = cost_function(cl);
        if ((cl_cost.routes).size() <= route_length_mother && cl_cost.fitness <= fit) {
            child2 = cl;
            fit = cl_cost.fitness;
        }
        if (child2.size() == 0) child2 = cl; //if routes is increasing irrespective of position
        mother = child2;
    }
    vector<vector<int>>children;
    children.push_back(child1);
    children.push_back(child2);

    //cout << "hi15" << endl;
    return children;

}

vector<vector<int>> Crossover_util(vector<vector<int>> &population, int father, int mother) {
    //cout << population.size() << endl;
    // cout << "population size: " << population.size() << endl;
    // random_device rd; // obtain a random number from hardware
    // mt19937 gen(rd()); // seed the generator
    // uniform_int_distribution<> distr(0, population.size() - 1);
    // //parents
    // int father = distr(gen);
    // // int mother = distr(gen);
    // while (father == mother) mother = distr(gen);
    //cout << "random number " << father << " " << mother << endl;

    Cost x = cost_function(population[father]);
    Cost y = cost_function(population[mother]);
    // cout << "hi1" << endl;
    //store the index of the genes of mother and father
    unordered_map<int, vector<int>>index_map;   // gene -> father index, mother index
    for (int i = 0; i < population[father].size(); i++) {
        index_map[population[father][i]].push_back(i);
    }
    for (int i = 0; i < population[mother].size(); i++) {
        index_map[population[mother][i]].push_back(i);
    }
    //cout << "hi2" << endl;

    // random_device rnd;
    // mt19937 genr(rnd());
    // uniform_int_distribution<> distri(0, x.routes.size() - 1);
    // uniform_int_distribution<> distrit(0, y.routes.size() - 1);
    // // select a random route to be deleted from other parent
    // int rf = distri(genr);
    // int rm = distrit(genr);
    int rf = random_vec(x.routes.size() - 1, 10)[0];
    int rm = random_vec(y.routes.size() - 1, 17)[0];
    // cout << x.routes.size() << " " << y.routes.size() << endl;
    // cout << rf << " " << rm << endl;
    vector<int>delete_element_index[2]; // father,mother
    vector<int>delete_element[2];       // father,mother
    if (rf == 0) {
        for (int i = 0; i < x.routes[rf]; i++) {
            delete_element[1].push_back(population[father][i]);
            delete_element_index[1].push_back(index_map[population[father][i]][1]);
        }
    }
    else {
        for (int i = x.routes[rf - 1]; i < x.routes[rf]; i++) {
            delete_element[1].push_back(population[father][i]);
            delete_element_index[1].push_back(index_map[population[father][i]][1]); //index of the element to be deleted in mother
        }
    }
    // cout << "hi3" << endl;
    if (rm == 0) {
        //cout << "hi1";
        for (int i = 0; i < y.routes[rm]; i++) {
            delete_element[0].push_back(population[mother][i]);
            delete_element_index[0].push_back(index_map[population[mother][i]][0]); //index of the element to be deleted in father
        }
    }
    else {
        //cout << "hey1";
        for (int i = y.routes[rm - 1]; i < y.routes[rm]; i++) {
            delete_element[0].push_back(population[mother][i]);
            delete_element_index[0].push_back(index_map[population[mother][i]][0]);
        }
    }
    //cout << "hi4" << endl;

    //parents after deleting the genes
    vector<int>parent_after_delete[2];
    parent_after_delete[0] = population[father];
    parent_after_delete[1] = population[mother];

    // cout << "hi5" << endl;
    for (int i = 0; i < delete_element_index[0].size(); i++) {
        parent_after_delete[0][delete_element_index[0][i]] = -1;
    }
    for (int i = 0; i < delete_element_index[1].size(); i++) {
        parent_after_delete[1][delete_element_index[1][i]] = -1;
    }

    //delete genes from parent
    //cout << "hi6" << endl;
    parent_after_delete[0].erase(remove(parent_after_delete[0].begin(), parent_after_delete[0].end(), -1), parent_after_delete[0].end());
    parent_after_delete[1].erase(remove(parent_after_delete[1].begin(), parent_after_delete[1].end(), -1), parent_after_delete[1].end());

    //store children generated after crossover
    //cout << "hi7" << endl;
    // for (auto x : parent_after_delete[0]) {
    //     cout << x << " " ;
    // }
    // cout << endl;
    // for (auto x : parent_after_delete[1]) {
    //     cout << x << " ";
    // }
    // cout << endl;
    vector<vector<int>>children = insert_util(parent_after_delete[0], parent_after_delete[1], delete_element, x.routes.size(), y.routes.size());
    //cout << "hi8" << endl;
    return children;
    // // cout << "children " << endl;
    //  for (int i = 0; i < children[0].size(); i++) cout << children[0][i] << " ";
    // // cout << endl;
    // // cout << "children " << endl;

    //  for (int i = 0; i < children[1].size(); i++) cout << children[1][i] << " ";
    //  // cout << endl;
    //  // cout << "population ";
    //  for (int i = 0; i < population[father].size(); i++) cout << population[father][i] << " ";
    //  // cout << endl;
    //  vector<int>vec = x.routes;
    //  //cout << "routes ";
    //  for (int i = 0; i < vec.size(); i++) cout << vec[i] << " ";
    //  // cout << endl;
    //  // cout << "num_vehicles " << x.num_vehicles << endl;


}

void Crossover(vector<vector<int>>& population) {
    int x = (population.size() * 8) / 10;
    if (x % 2 == 1) x--;
    vector<int> random_parents = random_vec(population.size() - 1, x);
    //cout << random_parents.size() << " random parents" << endl;

    for (int i = 0; i < random_parents.size(); i = i + 2)
    {
        // cout << i << endl;
        auto children = Crossover_util(population, random_parents[i], random_parents[i + 1]);
        //cout << children.size() << endl;
        population.push_back(children[0]);
        population.push_back(children[1]);
    }
}

int main() {

    Num_Customers = 100;
    Num_Chromosomes = 100;

    alpha = 100;
    beta = 1;

    take_input();

    vector<vector<int>> init_pop = Generate_initial_population();



    // cout << "initial population " << init_pop.size() << endl;
    //cout << "Mutation called" << endl;
    int gen = 0;
    while (gen < 35) {
        auto temp = init_pop;
        Crossover(temp);
        Mutation(temp);

        vector<pair<int, float>> costs(temp.size());
        for (int i = 0; i < temp.size(); i++) {
            auto  cost = cost_function(temp[i]);
            costs[i] = {cost.num_vehicles, cost.dist};
            // cout << cost.num_vehicles << " " << cost.dist << " " << cost.fitness << endl;
        }

        auto rank = pareto_ranking(costs);

        vector<pair<int, vector<int>>> vpp(temp.size());
        for (int i = 0; i < temp.size(); i++) {
            vpp[i] = {rank[i], temp[i]};
        }
        sort(vpp.begin(), vpp.end());

        for (int i = 0; i < Num_Customers; i++) {
            init_pop[i] = vpp[i].second;
        }

        gen++;
    }


    vector<pair<int, float>> costs(init_pop.size());
    for (int i = 0; i < init_pop.size(); i++) {
        auto  cost = cost_function(init_pop[i]);
        costs[i] = {cost.num_vehicles, cost.dist};
        // cout << cost.num_vehicles << " " << cost.dist << " " << cost.fitness << endl;
    }

    auto rank = pareto_ranking(costs);

    for (int i = 0; i < init_pop.size(); i++) {
        if (rank[i] == 1) {
            cout << costs[i].first << " " << costs[i].second << endl;
        }
    }

}