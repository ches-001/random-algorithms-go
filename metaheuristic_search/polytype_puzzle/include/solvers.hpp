#ifndef SOLVER_H
#define SOLVER

#include <iostream>
#include "custom_templates.hpp"


struct node_struct {
        int heuristics;
        vec_t<char> state;
        vec_t<std::string> path;
        
        node_struct();
        node_struct(int heuristics, vec_t<char> state, vec_t<std::string> path);
        int get_score()const;
        bool operator==(const node_struct &node)const;
        bool operator!=(const node_struct &node)const;
        bool operator<(const node_struct &node)const;
        bool operator>(const node_struct &node)const;
        bool operator<=(const node_struct &node)const;
        bool operator>=(const node_struct &node)const;
    };

class PolytypePuzzle{
    private:
        vec_t<char> current_state;

        vec_t<char>split_state_by_char(const std::string &val, char delim)const;
        void include_inverse_actions(json &action_map);

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
        );
        vec_t<char> step(const std::string &action);
        vec_t<char> get_current_state()const;
        void reset();
        void reset(const vec_t<char> &state);
        void reset(const std::string &state);
        int base_heuristics_fn(const vec_t<char> &state)const;
        int base_heuristics_fn(const std::string &state)const;
};

vec_t<std::string> astar_solver(PolytypePuzzle &puzzle, int max_depth);
vec_t<std::string> iterative_deepening_astar_solver(PolytypePuzzle &puzzle);
vec_t<std::string> beam_search_solver(PolytypePuzzle &puzzle, int beam_width);

#endif