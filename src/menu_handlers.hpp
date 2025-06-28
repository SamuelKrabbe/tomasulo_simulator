#pragma once

#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/place.hpp>
#include<nana/gui.hpp>
#include<nana/gui/widgets/button.hpp>
#include<nana/gui/widgets/menubar.hpp>
#include<nana/gui/widgets/group.hpp>
#include<nana/gui/widgets/textbox.hpp>
#include<nana/gui/filebox.hpp>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "gui.hpp"
#include "top.hpp"

using namespace nana;

void setup_main_menu(
    nana::menu& options_menu, bool& spec, int& mode,
    nana::place& plc, nana::form& fm,
    int& bpb_size, int& n_bits, int& nadd, int& nmul, int& nls, int& cpu_freq,
    std::map<std::string, int>& instruct_time,
    std::vector<std::string>& instruction_queue,
    std::ifstream& inFile, bool& fila,
    nana::listbox& instruct, nana::listbox& reg, nana::grid& memory,
    std::string& bench_name,
    nana::menu*& spec_sub_menu, nana::menu*& config_sub,
    nana::menu*& verify_sub, nana::menu*& bench_sub, 
    const char **argv
);
void setup_spec_menu(menu& spec_sub, bool& spec, int& mode, place& plc);
void setup_config_menu(
    menu& config_sub, form& fm, int& bpb_size, int& n_bits,
    int& nadd, int& nmul, int& nls, int& cpu_freq,
    std::map<std::string, int>& instruct_time,
    std::vector<std::string>& instruction_queue,
    std::ifstream& inFile, bool& fila,
    listbox& instruct,
    listbox& reg,
    grid& memory
);
void setup_verification_menu(
    menu& verify_sub, form& fm,
    std::ifstream& inFile,
    listbox& reg,
    grid& memory
);
void setup_benchmark_menu(
    menu& bench_sub, string& bench_name, form& fm, 
    std::vector<std::string>& instruction_queue,
    std::ifstream& inFile, bool& fila, listbox& instruct,
    listbox& reg, grid& memory, const char **argv
);
