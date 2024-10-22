#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <iostream>
#include <fstream>
#include "custom_templates.hpp"

uo_vec_map_t<std::string, std::string> *parse_csv_file(std::ifstream &file);

#endif