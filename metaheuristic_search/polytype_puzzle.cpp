#include <iostream>
#include <cmath>
#include <algorithm>
#include <chrono>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <set>
#include "3pincludes/json.hpp"

template <typename T> using vec_t = std::vector<T>;
template <typename T> using mat_t = std::vector<std::vector<T>>;
template <typename keyT, typename VecValT> using uo_vec_map_t = std::unordered_map<keyT, std::vector<VecValT>>;
using json = nlohmann::json;

struct node_struct {
        int heuristics;
        vec_t<char> state;
        vec_t<std::string> path;
        
        node_struct() : heuristics(), state(), path() {}

        node_struct(
            int heuristics, 
            vec_t<char> state, 
            vec_t<std::string> path
        ) : heuristics(heuristics), state(state), path(path) {}

        int get_score()const{
            return this->heuristics + (int) this->path.size();
        }
        bool operator==(const node_struct &node)const{
            return (
                this->heuristics == node.heuristics &&
                this->state == node.state &&
                this->path == node.path
            );
        }
        bool operator!=(const node_struct &node)const{
            return !(*this == node);
        }
        bool operator<(const node_struct &node)const{
            return (this->get_score()) < (node.get_score());
        }
        bool operator>(const node_struct &node)const{
            return (this->get_score()) > (node.get_score());
        }
        bool operator<=(const node_struct &node)const{
            return !(*this > node);
        }
        bool operator>=(const node_struct &node)const{
            return !(*this < node);
        }
    };

class PolytypePuzzle{
    private:
        vec_t<char> current_state;

        vec_t<char>split_state_by_char(const std::string &val, char delim)const{
            vec_t<char> result;
            int prev = 0, pos = 0;
            char c;
            while(true){
                pos = val.find(delim, pos);
                if (pos == std::string::npos) {
                    c = val.substr(prev+1, 1)[0];
                    result.emplace_back((c!=delim) ? c : val.substr(pos, 0)[0]);
                    break;
                };
                c = ((pos > 0) ? val.substr(pos-1, 1) : val.substr(pos, 0))[0];
                result.emplace_back((c!=delim) ? c : val.substr(pos, 0)[0]);
                prev = pos;
                pos += 1;
            }
            return result;
        };

        void include_inverse_actions(json &action_map){
            vec_t<int> inv_action_vals;
            vec_t<int> action_vals;
            json new_action_map;
            std::string _tempk;
            int c = 0;
            for(auto itr=action_map.begin(); itr != action_map.end(); ++itr){
                _tempk = itr.key();
                action_vals = action_map[_tempk].get<vec_t<int>>();
                if (c == 0){
                    for(int i=0; i < action_vals.size(); i++){
                        inv_action_vals.push_back(i);
                    }
                }
                std::sort(
                    inv_action_vals.begin(),
                    inv_action_vals.end(),
                    [&action_vals](int a, int b){
                        return action_vals[a] < action_vals[b];
                    }
                );
                c++;
                new_action_map["-"+_tempk] = inv_action_vals;
            }
            for(auto itr=new_action_map.begin(); itr != new_action_map.end(); ++itr){
                _tempk = itr.key();
                action_map[_tempk] = new_action_map[_tempk];
            }
        }
        
    public:
        vec_t<char> start;
        vec_t<char> goal;
        json actions_map;
        int num_wildcard;
        char state_delim;

        PolytypePuzzle(
            std::string &start,
            std::string &goal, 
            std::string &actions_map,
            int num_wildcard,
            char state_delim=';'
        ){
            this->state_delim = state_delim;
            this->start = this->split_state_by_char(start, this->state_delim);
            this->goal = this->split_state_by_char(goal, this->state_delim);
            this->current_state = this->start;
            this->actions_map = json::parse(actions_map);
            this->include_inverse_actions(this->actions_map);
            this->num_wildcard = num_wildcard;
        }

        vec_t<char> step(const std::string &action){
            vec_t<int> action_val = actions_map[action];
            vec_t<char> new_state(start.size());
            std::transform(
                action_val.begin(), 
                action_val.end(), 
                new_state.begin(), 
                [this](int act_idx){
                    return this->current_state[act_idx];
                }
            );
            this->current_state = new_state;
            return new_state;
        }

        vec_t<char> get_current_state()const{
            return this->current_state;
        }

        void reset(){
            this->current_state = this->start;
        }

        void reset(const vec_t<char> &state){
            this->current_state = state;
        }
        
        void reset(const std::string &state){
            reset(this->split_state_by_char(state, this->state_delim));
        }

        int base_heuristics_fn(const vec_t<char> &state)const{
            int i = 0;
            int correct = 0;
            vec_t<char> _temp(state.size());
            std::transform(
                state.begin(), 
                state.end(), 
                _temp.begin(), 
                [&i, &correct, this](char s){
                    bool val = s == this->goal[i];
                    correct += val ? 1 : 0;
                    i++;
                    return s;
                }
            );
            int mistakes = ((int) state.size() - correct) - this->num_wildcard;
            return mistakes;
        }
        
        int base_heuristics_fn(const std::string &state)const{
            vec_t<char> _state = this->split_state_by_char(state, this->state_delim);
            return base_heuristics_fn(_state);
        }
};

uo_vec_map_t<std::string, std::string> *parse_csv_file(std::ifstream &file);
vec_t<std::string> astar_solver(PolytypePuzzle &puzzle, int max_depth);
vec_t<std::string> iterative_deepening_astar_solver(PolytypePuzzle &puzzle);
vec_t<std::string> beam_search_solver(PolytypePuzzle &puzzle, int beam_width);

int main(){
    std::string puzzles_info_csv("data/puzzle_info.csv");
    std::string puzzles_csv("data/puzzles.csv");
    std::ifstream puzzles_info(puzzles_info_csv);
    std::ifstream puzzles(puzzles_csv);

    uo_vec_map_t<std::string, std::string> *puzzle_map = parse_csv_file(puzzles);
    uo_vec_map_t<std::string, std::string> *puzzle_info_map = parse_csv_file(puzzles_info);

    int num_wildcards;
    std::string puzzle_type, start, goal, allowed_moves;
    vec_t<std::string> solution;

    int _icount=284, _istop=328;
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
    return 0;
}
 
uo_vec_map_t<std::string, std::string> *parse_csv_file(std::ifstream &file){
    int i = 0, j = 0;
    int pos, prev, qprev, next_qpos;
    std::string line, subline;
    vec_t<std::string> row, header;
    uo_vec_map_t<std::string, std::string> *results = new uo_vec_map_t<std::string, std::string>();

    auto replace = [](std::string &val, std::string to_replace, std::string replace_with){
        int pos=0;
        while(true){
            pos = val.find(to_replace, pos);
            if(pos == std::string::npos) break;
            val.replace(pos, to_replace.length(), replace_with);
            pos += to_replace.length();
        }
    };

    while(std::getline(file, line)){
        j = 0; pos = 0; prev = 0; next_qpos = 0;
        subline.clear();
        row.clear();
        do{
            pos = line.find(",", prev);
            subline = line.substr(prev, pos-prev);
            if(i == 0){
                results->insert({subline, {}});
                header.emplace_back(subline);
            }
            else{
                if(subline[0] == '"'){
                    next_qpos = line.find('"', prev+1);
                    subline = line.substr(prev+1, next_qpos-prev-1);
                    replace(subline, "'", "\"");
                    pos = next_qpos;
                }
                if(row.size() < header.size()){
                    row.emplace_back(subline);
                }else{
                    break;
                }
            }
            prev = pos + 1;
        }while(pos != std::string::npos); 
        i++;
        for(std::string &v : row){
            (*results)[header[j]].push_back(v);
            j++;
        }
    }
    return results;    
}

vec_t<std::string> astar_solver(PolytypePuzzle &puzzle, int max_depth){
    puzzle.reset();
    vec_t<node_struct> openset;
    std::make_heap(openset.begin(), openset.end(), std::greater<>());
    std::set<vec_t<char>> closed_set;
    node_struct current_node;
    vec_t<char> child_state = puzzle.get_current_state();
    int child_heuristics = puzzle.base_heuristics_fn(child_state);
    vec_t<std::string> child_path = {};
    std::string _actk;
    openset.emplace_back(child_heuristics, child_state, child_path);
    std::push_heap(openset.begin(), openset.end(), std::greater<>());
    
    while(true){
        std::pop_heap(openset.begin(), openset.end(), std::greater<>());
        current_node = openset.back();
        openset.pop_back();
        puzzle.reset(current_node.state);
        if (puzzle.base_heuristics_fn(current_node.state) == 0){
            return current_node.path;
        }
        if (
            closed_set.count(current_node.state) > 0 || 
            (max_depth > 0 && current_node.path.size() >= max_depth)
        ){
            continue;
        }
        closed_set.insert(current_node.state);
        child_path.insert(child_path.end(), current_node.path.begin(), current_node.path.end());
        for(auto itr=puzzle.actions_map.begin(); itr!=puzzle.actions_map.end(); ++itr){
            _actk = itr.key();
            child_state = puzzle.step(_actk);
            child_path.push_back(_actk);
            child_heuristics = puzzle.base_heuristics_fn(child_state);
            openset.emplace_back(child_heuristics, child_state, child_path);
            std::push_heap(openset.begin(), openset.end(), std::greater<>());
            child_path.pop_back();
            puzzle.reset(current_node.state);
        }
        child_path.clear();
    }
}

vec_t<std::string> iterative_deepening_astar_solver(PolytypePuzzle &puzzle){
    puzzle.reset();
    vec_t<char> initial_state = puzzle.get_current_state();
    int initial_score = puzzle.base_heuristics_fn(initial_state);
    int score_threshold = initial_score;
    vec_t<std::string> initial_path = {};
    node_struct root_node(initial_score, initial_state, initial_path);
    node_struct max_or_target_node;

    std::function<node_struct(node_struct)> expand_node = [
        &expand_node, &puzzle, &score_threshold
    ](const node_struct &node){
        if(node.get_score() > score_threshold || node.heuristics == 0){
            return node;
        }
        node_struct max_node;
        node_struct pruned_node;
        int child_heuristics;
        vec_t<char> child_state;
        vec_t<std::string> child_path = node.path;
        vec_t<node_struct> children_nodes(puzzle.actions_map.size());
        int i = 0;

        std::string _actk;
        puzzle.reset(node.state);
        for(auto itr=puzzle.actions_map.begin(); itr!=puzzle.actions_map.end(); ++itr){
            _actk = itr.key();
            child_state = puzzle.step(_actk);
            child_path.push_back(_actk);
            child_heuristics = puzzle.base_heuristics_fn(child_state);
            children_nodes[i] = node_struct(child_heuristics, child_state, child_path);
            child_path.pop_back();
            puzzle.reset(node.state);
            i++;
        }

        for(node_struct &child : children_nodes){
            pruned_node = expand_node(child);
            if (pruned_node.heuristics == 0){
                return pruned_node;
            }
            if(pruned_node > max_node){
                max_node = pruned_node;
            }
        }
        return max_node;
    };
    
    while(true){
        max_or_target_node = expand_node(root_node);
        if(max_or_target_node.heuristics == 0){
            return max_or_target_node.path;
        }
        score_threshold = max_or_target_node.get_score();
    }
}

vec_t<std::string> beam_search_solver(PolytypePuzzle &puzzle, int beam_width){
    puzzle.reset();
    vec_t<char> initial_state = puzzle.get_current_state();
    int initial_score = puzzle.base_heuristics_fn(initial_state);
    int score_threshold = initial_score;
    vec_t<std::string> initial_path = {};
    node_struct root_node(initial_score, initial_state, initial_path);
    vec_t<node_struct> children;
    vec_t<node_struct> best_children(beam_width);

    auto expand_node = [&puzzle, &children](node_struct &node){
        std::string _actk;
        int child_heuristics;
        vec_t<char> child_state;
        vec_t<std::string> child_path = node.path;
        puzzle.reset(node.state);
        for(auto itr=puzzle.actions_map.begin(); itr!=puzzle.actions_map.end(); ++itr){
            _actk = itr.key();
            child_state = puzzle.step(_actk);
            child_heuristics = puzzle.base_heuristics_fn(child_state);
            child_path.push_back(_actk);
            children.emplace_back(child_heuristics, child_state, child_path);
            child_path.pop_back();
            puzzle.reset(node.state);
        }
    };
    auto set_best_children = [
        &best_children, &beam_width
    ](vec_t<node_struct> &children){
        std::sort(children.begin(), children.end(), std::less<node_struct>());
        for(int i=0; i<beam_width; i++){
            best_children[i] = children[i];
        }
    };
    int num_iters = 0;
    while(true){
        if(num_iters == 0){
            expand_node(root_node);
        }
        set_best_children(children);
        children.clear();
        for(node_struct &child : best_children){
            if(child.heuristics == 0){
                return child.path;
            }
            expand_node(child);
        }
        num_iters++;
    }
}