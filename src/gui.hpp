#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include<nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/button.hpp>
#include "top.hpp"
#include "grid.hpp"

using namespace nana;

extern const char* str_spec; 

extern const char*  str_nospec; 

void set_spec(place &plc, bool is_spec);
bool add_instructions(ifstream &File, vector<std::string> &queue, listbox &instruction_gui);
void show_message(std::string message_title, std::string message);
void show_metrics_window(form& parent, const std::string& metrics_text);

// Setup dos Headers
void setup_register_headers(listbox& reg);
void setup_table_headers(listbox& table);
void setup_instruction_headers(listbox& instruct);
void setup_rob_headers(listbox& rob);

// Setup dos dados
void fill_registers(listbox& reg);
void fill_memory(grid& memory);
void parse_arguments(
    int argc, char* argv[],
    vector<std::string>& instruction_queue,
    listbox& instruct,
    listbox& reg,
    grid& memory,
    std::map<std::string,int>& instruct_time,
    int& nadd, int& nmul, int& nls,
    bool& fila,
    form& fm
);
