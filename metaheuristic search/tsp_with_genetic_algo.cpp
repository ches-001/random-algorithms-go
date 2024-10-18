#include <iostream>
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

typedef std::vector<int> tsp_vector_t;
typedef std::vector<std::vector<int>> tsp_matrix_t;

tsp_matrix_t create_tsp_matrix();

int evaluate(tsp_vector_t &candidate, tsp_matrix_t &tsp_matrix);

tsp_matrix_t cross(tsp_vector_t &parent1, tsp_vector_t &parent2);

void mutate(std::mt19937 gen, tsp_vector_t &candidate, double p);

tsp_matrix_t init_population(std::mt19937 &gen, int pop_size, int num_locs, int start_loc);

tsp_vector_t evaluate_population(tsp_matrix_t &population, tsp_matrix_t &tsp_matrix);

tsp_matrix_t cross_population(tsp_matrix_t &parents);

void mutate_population(std::mt19937 gen, tsp_matrix_t &population, double p);


int main(){
    const int POPULATION_SIZE = 500;
    const int NUM_ITERS = 100;
    const int NUM_SELECTIONS = 100;
    const double MUTATION_PROBA = 0.2;
    const int START_LOC = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    tsp_matrix_t tsp_matrix = create_tsp_matrix();
    std::size_t tsp_rows(tsp_matrix.size()), tsp_cols(tsp_matrix[0].size());
    tsp_matrix_t population = init_population(gen, POPULATION_SIZE, tsp_rows, START_LOC);
    std::size_t pop_size = population.size();
    tsp_vector_t score_indexes(pop_size);
    tsp_matrix_t best_candidates(NUM_SELECTIONS);
    int num_offsprings = (NUM_SELECTIONS % 2 == 0) ? NUM_SELECTIONS : NUM_SELECTIONS+1;
    tsp_matrix_t offsprings(num_offsprings);
    tsp_vector_t elite_candidate;
    int elite_score;

    auto start = std::chrono::high_resolution_clock::now();
    
    //populate score indexes
    for(int j=0; j<pop_size; j++){score_indexes[j]=j;}
    
    for(int i=0; i<NUM_ITERS; i++){
        tsp_vector_t scores = evaluate_population(population, tsp_matrix);
        std::size_t pop_size = population.size();
        std::sort(
            score_indexes.begin(), 
            score_indexes.end(), 
            [&scores](int a, int b){return scores[a] < scores[b];}
        );
        elite_candidate = population[score_indexes[0]];
        elite_score = scores[score_indexes[0]];
        std::transform(
            score_indexes.begin(), 
            score_indexes.begin()+NUM_SELECTIONS,
            best_candidates.begin(), 
            [&population](int idx){
                return population[idx];
            }
        );
        offsprings = cross_population(best_candidates);
        mutate_population(gen, offsprings, MUTATION_PROBA);

        for(int j=static_cast<int>(score_indexes.size()-offsprings.size()); j<score_indexes.size(); j++){
            population[score_indexes[j]] = offsprings[j - static_cast<int>(pop_size - offsprings.size())];
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = (end - start);

    for(int &loc : elite_candidate){
        std::cout << loc << "-";
    }
    std::cout << elite_candidate[0] << "-";
    std::cout << "\tbest score: " << elite_score << std::endl; 
    printf("duration: %f sec\n", duration);
    return 0;
}

tsp_matrix_t create_tsp_matrix(){
    int inv = 999;
    tsp_matrix_t mat = {
        // A, B, C, D, E, F
        {0, 10, inv, inv, inv, 6},    // A
        {10, 0, 19, inv, 10, inv},    // B
        {inv, 19, 0, 22, inv, 15},    // C
        {inv, inv, 22, 0, 5, inv},    // D
        {inv, 10, inv, 5, 0, 12},     // E
        {6, inv, 15, inv, 12, 0},     // F
    };
    return mat;
}

int evaluate(tsp_vector_t &candidate, tsp_matrix_t &tsp_matrix){
    std::size_t len = candidate.size();
    int dist = 0;
    int i=0;
    for(; i<len-1; i++){
        dist += tsp_matrix[candidate[i]][candidate[i+1]];
    }
    dist += tsp_matrix[candidate[i]][candidate[0]];
    return dist;
}

tsp_matrix_t cross(tsp_vector_t &parent1, tsp_vector_t &parent2){
    std::size_t _csize = parent1.size();
    int _midp = std::floor(_csize / 2);
    tsp_vector_t offspring1, offspring2;

    offspring1.resize(_midp);
    std::copy(parent1.begin(), parent1.begin()+_midp, offspring1.begin());

    offspring2.resize(_midp);
    std::copy(parent2.begin(), parent2.begin()+_midp, offspring2.begin());

    for(int j=0; j<_csize; j++){
        if(std::count(offspring1.begin(), offspring1.end(), j) == 0){
            offspring1.push_back(j);
        }
        if(std::count(offspring2.begin(), offspring2.end(), j) == 0){
            offspring2.push_back(j);
        }
    }
    tsp_matrix_t offsprings = {offspring1, offspring2};
    return offsprings;
}

void mutate(std::mt19937 gen, tsp_vector_t &candidate, double p){
    std::uniform_real_distribution<double>dist(0.0, 1.0);
    double rand_num = dist(gen);
    if(p > rand_num){
        std::shuffle(candidate.begin()+1, candidate.end(), gen);
    }    
}

tsp_matrix_t init_population(std::mt19937 &gen, int pop_size, int num_locs, int start_loc){
    tsp_vector_t loc_ids(num_locs);
    tsp_matrix_t population(pop_size);
    
    for(int i=0; i<num_locs; i++){
        loc_ids[i] = i;
    }
    loc_ids[0] = start_loc;
    loc_ids[start_loc] = 0;
    for (int i=0; i<pop_size; i++){
        tsp_vector_t _temp = loc_ids;
        std::shuffle(_temp.begin()+1, _temp.end(), gen);
        population[i] = _temp;
    }
    return population;
}

tsp_vector_t evaluate_population(tsp_matrix_t &population, tsp_matrix_t &tsp_matrix){
    std::size_t pop_size = population.size();
    tsp_vector_t scores(pop_size);
    std::transform(
        population.begin(), 
        population.end(), 
        scores.begin(), 
        [&tsp_matrix](tsp_vector_t candidate){
            return evaluate(candidate, tsp_matrix);
        }
    );
    return scores;
}
// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
tsp_matrix_t cross_population(tsp_matrix_t &parents){
    int size = (int) parents.size();
    int num_iters = (int) std::floor(size / 2);
    tsp_matrix_t offsprings((size % 2 == 0) ? size : size+1);
    int i = 0; 
    for(; i<num_iters; i++){
        tsp_vector_t p1 = parents[i]; 
        tsp_vector_t p2 = parents[size-1-i];
        tsp_matrix_t _offsprings_1_2 = cross(p1, p2);
        std::transform(
            _offsprings_1_2.begin(),
            _offsprings_1_2.end(), 
            offsprings.begin()+(i*2), 
            [](tsp_vector_t x) {return x;}
        );

    }
    if (((double) size / 2) > num_iters){
        tsp_matrix_t _offsprings_1_2 = cross(parents[rand() % size], parents[i+1]);
        std::transform(
            _offsprings_1_2.begin(),
            _offsprings_1_2.end(), 
            offsprings.begin()+(i*2), 
            [](tsp_vector_t x) {return x;}
        );
    }
    return offsprings;
}

void mutate_population(std::mt19937 gen, tsp_matrix_t &population, double p){
    std::size_t pop_size = population.size();
    for(int i=0; i<pop_size; i++){
        mutate(gen, population[i], p);
    }
}