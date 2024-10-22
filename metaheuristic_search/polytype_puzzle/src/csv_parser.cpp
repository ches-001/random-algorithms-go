#include <iostream>
#include <fstream>
#include "../include/csv_parser.hpp"
#include "../include/custom_templates.hpp"

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