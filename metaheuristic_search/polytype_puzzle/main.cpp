#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <set>
//Nlohmann JSON
#include "include/json.hpp"
//Custom includes
#include "include/csv_parser.hpp"
#include "include/solvers.hpp"
#include "include/custom_templates.hpp"


int main(){
    std::string puzzles_info_csv("../data/puzzle_info.csv");
    std::string puzzles_csv("../data/puzzles.csv");
    std::ifstream puzzles_info(puzzles_info_csv);
    std::ifstream puzzles(puzzles_csv);

    if(!puzzles_info.is_open()){
        std::cerr << "error opening " << puzzles_info_csv << std::endl;
    }
     if(!puzzles.is_open()){
        std::cerr << "error opening " << puzzles_csv << std::endl;
    }
            
    uo_vec_map_t<std::string, std::string> *puzzle_map = parse_csv_file(puzzles);
    uo_vec_map_t<std::string, std::string> *puzzle_info_map = parse_csv_file(puzzles_info);

    int num_wildcards;
    std::string puzzle_type, start, goal, allowed_moves;
    vec_t<std::string> solution;

    int _icount=284, _istop=336;
    for (;_icount<_istop; _icount++){
        puzzle_type = (*puzzle_map)["puzzle_type"][_icount];
        start = (*puzzle_map)["initial_state"][_icount];
        goal = (*puzzle_map)["solution_state"][_icount];
        num_wildcards = std::stoi((*puzzle_map)["num_wildcards"][_icount]);

        for(int i=0; i<(*puzzle_info_map)["puzzle_type"].size(); i++){
            if((*puzzle_info_map)["puzzle_type"][i] == puzzle_type){
                allowed_moves = (*puzzle_info_map)["allowed_moves"][i];
                break;
            }
        }
        PolytypePuzzle puzzle(start, goal, allowed_moves, num_wildcards);
        auto start = std::chrono::high_resolution_clock::now();
        solution = astar_solver(puzzle, -1);
        // solution = iterative_deepening_astar_solver(puzzle);
        // solution = beam_search_solver(puzzle, 4);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = (end - start);
        std::cout << "puzzle id: " << _icount << "\tpuzzle_type: " << puzzle_type << std::endl;
        std::cout << "solution: ";
        for(std::string &action : solution){
            std::cout << action << " ";
        }
        std::cout << std::endl;
        printf("duration: %f sec\n\n", duration);
    }

    delete puzzle_map;
    delete puzzle_info_map;
    puzzle_map = nullptr;
    puzzle_info_map = nullptr;
    return 0;
}