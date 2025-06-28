#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/msgbox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/group.hpp>
#include <sstream>
#include "gui.hpp"
#include "top.hpp"
#include "menu_handlers.hpp"
#include <unistd.h>
#include <libgen.h>

using namespace nana;

const char* get_base_path(const char **argv) {
    // 1. Try environment variable first
    const char* env_path = getenv("TFSIM_BASE_PATH");
    if (env_path != NULL && env_path[0] != '\0') {
        return env_path;
    }

    // 2. Fall back to argv[0] derivation
    static char base_path[PATH_MAX];
    char executable_path[PATH_MAX];

    // Get path to executable (Linux/macOS specific)
    ssize_t len = readlink("/proc/self/exe", executable_path, sizeof(executable_path)-1);
    if (len == -1) {
        // Fallback for systems without /proc/self/exe
        strncpy(executable_path, argv[0], sizeof(executable_path)-1);
        executable_path[sizeof(executable_path)-1] = '\0';
    } else {
        executable_path[len] = '\0';
    }

    // Get directory containing executable
    char* dir = dirname(executable_path);
    strncpy(base_path, dir, sizeof(base_path)-1);
    base_path[sizeof(base_path)-1] = '\0';

    return base_path;
}

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
) {
    setup_spec_menu(*spec_sub_menu, spec, mode, plc);

    setup_config_menu(*config_sub, fm, bpb_size, n_bits, nadd, nmul, nls, cpu_freq,
                      instruct_time, instruction_queue, inFile, fila, instruct, reg, memory);

    setup_verification_menu(*verify_sub, fm, inFile, reg, memory);

    setup_benchmark_menu(*bench_sub, bench_name, fm, instruction_queue,
                         inFile, fila, instruct, reg, memory, argv);
}

void setup_spec_menu(menu& spec_sub, bool& spec, int& mode, place& plc) {
    spec_sub.append("1 Preditor", [&](menu::item_proxy& ip) {
        if(ip.checked()) {
            spec = true;
            mode = 1;
            spec_sub.checked(1, false);
        } else {
            spec = false;
            mode = 0;
        }
        set_spec(plc, spec);
    });

    spec_sub.append("Branch Prediction Buffer", [&](menu::item_proxy& ip) {
        if(ip.checked()) {
            spec = true;
            mode = 2;
            spec_sub.checked(0, false);
        } else {
            spec = false;
            mode = 0;
        }
        set_spec(plc, spec);
    });

    spec_sub.check_style(0, menu::checks::highlight);
    spec_sub.check_style(1, menu::checks::highlight);
}

void setup_config_menu(
    menu& config_sub, form& fm, int& bpb_size, int& n_bits,
    int& nadd, int& nmul, int& nls, int& cpu_freq,
    map<string, int>& instruct_time,
    vector<string>& instruction_queue,
    ifstream& inFile, bool& fila,
    listbox& instruct,
    listbox& reg,
    grid& memory
)
{
    config_sub.append("Tamanho do BPB e Preditor", [&](menu::item_proxy&) {
        inputbox ibox(fm, "", "Definir tamanhos");
        inputbox::integer size("BPB", bpb_size, 2, 10, 2);
        inputbox::integer bits("N_BITS", n_bits, 1, 3, 1);
        if (ibox.show_modal(size, bits)) {
            bpb_size = size.value();
            n_bits = bits.value();
        }
    });

    config_sub.append("Número de Estações de Reserva", [&](menu::item_proxy&) {
        inputbox ibox(fm, "", "Quantidade de Estações de Reserva");
        inputbox::integer add("ADD/SUB", nadd, 1, 10, 1);
        inputbox::integer mul("MUL/DIV", nmul, 1, 10, 1);
        inputbox::integer sl("LOAD/STORE", nls, 1, 10, 1);
        if (ibox.show_modal(add, mul, sl)) {
            nadd = add.value();
            nmul = mul.value();
            nls = sl.value();
        }
    });

    config_sub.append("Tempos de latência", [&](menu::item_proxy&) {
        inputbox ibox(fm,"","Tempos de latência para instruções");
        inputbox::text dadd_t("DADD",std::to_string(instruct_time["DADD"]));
        inputbox::text daddi_t("DADDI",std::to_string(instruct_time["DADDI"]));
        inputbox::text dsub_t("DSUB",std::to_string(instruct_time["DSUB"]));
        inputbox::text dsubi_t("DSUBI",std::to_string(instruct_time["DSUBI"]));
        inputbox::text dmul_t("DMUL",std::to_string(instruct_time["DMUL"]));
        inputbox::text ddiv_t("DDIV",std::to_string(instruct_time["DDIV"]));
        inputbox::text mem_t("Load/Store",std::to_string(instruct_time["MEM"]));
        if(ibox.show_modal(dadd_t,daddi_t,dsub_t,dsubi_t,dmul_t,ddiv_t,mem_t))
        {
            instruct_time["DADD"] = std::stoi(dadd_t.value());
            instruct_time["DADDI"] = std::stoi(daddi_t.value());
            instruct_time["DSUB"] = std::stoi(dsub_t.value());
            instruct_time["DSUBI"] = std::stoi(dsubi_t.value());
            instruct_time["DMUL"] = std::stoi(dmul_t.value());
            instruct_time["DDIV"] = std::stoi(ddiv_t.value());
            instruct_time["MEM"] = std::stoi(mem_t.value());
        }
    });

    config_sub.append("Frequência CPU", [&](menu::item_proxy&) {
        inputbox ibox(fm, "Em Mhz", "Definir frequência da CPU");
        inputbox::text freq("Frequência", std::to_string(cpu_freq));
        if (ibox.show_modal(freq)) {
            cpu_freq = std::stoi(freq.value());
        }
    });

    config_sub.append("Fila de instruções", [&](menu::item_proxy&) {
        filebox fb(0, true);
        inputbox ibox(fm, "Localização do arquivo com a lista de instruções:");
        inputbox::path caminho("", fb);
        if (ibox.show_modal(caminho)) {
            auto path = caminho.value();
            inFile.open(path);
            if (!add_instructions(inFile, instruction_queue, instruct))
                show_message("Arquivo inválido", "Não foi possível abrir o arquivo!");
            else
                fila = true;
        }
    });

    config_sub.append("Valores de registradores inteiros", [&](menu::item_proxy&) {
        filebox fb(0, true);
        inputbox ibox(fm, "Localização do arquivo de registradores inteiros:");
        inputbox::path caminho("", fb);
        if (ibox.show_modal(caminho)) {
            auto path = caminho.value();
            inFile.open(path);
            if (!inFile.is_open()) {
                show_message("Arquivo inválido", "Não foi possível abrir o arquivo!");
            } else {
                auto reg_gui = reg.at(0);
                int value, i = 0;
                while (inFile >> value && i < 32)
                    reg_gui.at(i++).text(1, std::to_string(value));
                for (; i < 32; ++i)
                    reg_gui.at(i).text(1, "0");
                inFile.close();
            }
        }
    });

    config_sub.append("Valores de memória", [&](menu::item_proxy&) {
        filebox fb(0, true);
        inputbox ibox(fm, "Localização do arquivo de valores de memória:");
        inputbox::path caminho("", fb);
        if (ibox.show_modal(caminho)) {
            auto path = caminho.value();
            inFile.open(path);
            if (!inFile.is_open()) {
                show_message("Arquivo inválido", "Não foi possível abrir o arquivo!");
            } else {
                int i = 0, value;
                while (inFile >> value && i < 500)
                    memory.Set(i++, std::to_string(value));
                for (; i < 500; ++i)
                    memory.Set(i, "0");
                inFile.close();
            }
        }
    });
}

void setup_verification_menu(menu& verify_sub, form& fm, ifstream& inFile,
                             listbox& reg, grid& memory) {
    verify_sub.append("Valores de registradores", [&](menu::item_proxy&) {
        filebox fb(0, true);
        inputbox ibox(fm, "Localização do arquivo de registradores:");
        inputbox::path caminho("", fb);
        if (ibox.show_modal(caminho)) {
            auto path = caminho.value();
            inFile.open(path);
            if (!inFile.is_open()) {
                show_message("Arquivo inválido", "Não foi possível abrir o arquivo!");
                return;
            }

            string str, value;
            int reg_pos, i;
            bool is_float, ok = true;
            auto reg_gui = reg.at(0);

            while (inFile >> str) {
                if (str[0] == '$') {
                    is_float = (str[1] == 'f');
                    i = is_float ? 2 : 1;
                    reg_pos = std::stoi(str.substr(i));
                    inFile >> value;
                    string current = is_float ? reg_gui.at(reg_pos).text(4) : reg_gui.at(reg_pos).text(1);
                    if ((is_float && std::stof(current) != std::stof(value)) ||
                        (!is_float && current != value)) {
                        ok = false;
                        break;
                    }
                }
            }
            inFile.close();

            msgbox msg("Verificação de registradores");
            msg << (ok ? "Registradores estão corretos." : "Diferença encontrada nos registradores!");
            msg.icon(ok ? msgbox::icon_information : msgbox::icon_error);
            msg.show();
        }
    });

    verify_sub.append("Valores de memória", [&](menu::item_proxy&) {
        filebox fb(0, true);
        inputbox ibox(fm, "Localização do arquivo de valores de memória:");
        inputbox::path caminho("", fb);
        if (ibox.show_modal(caminho)) {
            auto path = caminho.value();
            inFile.open(path);
            if (!inFile.is_open()) {
                show_message("Arquivo inválido", "Não foi possível abrir o arquivo!");
                return;
            }

            string value;
            bool ok = true;
            for(int i = 0 ; i < 500 ; i++) {
                inFile >> value;
                if(std::stoi(memory.Get(i)) != (int)std::stol(value,nullptr,16))
                {
                    ok = false;
                    break;
                }
            }
            inFile.close();

            msgbox msg("Verificação de memória");
            msg << (ok ? "Endereços de memória estão corretos." : "Diferença encontrada nos endereços de memória!");
            msg.icon(ok ? msgbox::icon_information : msgbox::icon_error);
            msg.show();
        }
    });
}
void setup_benchmark_menu(
    menu& bench_sub, string& bench_name, form& fm, 
    std::vector<std::string>& instruction_queue,
    std::ifstream& inFile, bool& fila, listbox& instruct,
    listbox& reg, grid& memory, const char **argv
) {
    std::string base_path = std::string(get_base_path(argv));

    bench_sub.append("All", [&](menu::item_proxy &ip) {
        struct Benchmark {
            std::string name;
            std::string instructionPath;
            std::string memoryPath;
            std::string regPath;
        };

        std::vector<Benchmark> benchmarks = {
            {"fibonacci", base_path + "in/benchmarks/fibonacci/fibonacci.txt", "", ""},
            {"division_stall", base_path + "in/benchmarks/division_stall.txt", "", ""},
            {"store_stress", base_path + "in/benchmarks/store_stress/store_stress.txt", "", ""},
            {"res_stations_stall", base_path + "in/benchmarks/res_stations_stall.txt", "", ""},
            {"linear_search", base_path + "in/benchmarks/linear_search/linear_search.txt", base_path + "in/benchmarks/linear_search/memory.txt", base_path + "in/benchmarks/linear_search/regi_i.txt"},
            {"binary_search", base_path + "in/benchmarks/binary_search/binary_search.txt", base_path + "in/benchmarks/binary_search/memory.txt", base_path + "in/benchmarks/binary_search/regs.txt"},
            {"matriz_search", base_path + "in/benchmarks/matriz_search/matriz_search.txt", base_path + "in/benchmarks/matriz_search/memory.txt", base_path + "in/benchmarks/matriz_search/regs.txt"},
            {"bubble_sort", base_path + "in/benchmarks/bubble_sort/bubble_sort.txt", base_path + "in/benchmarks/bubble_sort/memory.txt", base_path + "in/benchmarks/bubble_sort/regs.txt"},
            {"insertion_sort", base_path + "in/benchmarks/insertion_sort/insertion_sort.txt", base_path + "in/benchmarks/insertion_sort/memory.txt", base_path + "in/benchmarks/insertion_sort/regs.txt"},
            {"tick_tack", base_path + "in/benchmarks/tick_tack/tick_tack.txt", "", base_path + "in/benchmarks/tick_tack/regs.txt"}
        };

        for (const auto &b : benchmarks) {
            bench_name = b.name;
            inFile.open(b.instructionPath);
            if (!add_instructions(inFile, instruction_queue, instruct)) {
                show_message("Erro", "Não foi possível abrir " + b.instructionPath);
                inFile.close();
                continue;
            } else {
                fila = true;
            }
            inFile.close();

            // Carrega memória se necessário
            if (!b.memoryPath.empty()) {
                inFile.open(b.memoryPath);
                if (inFile.is_open()) {
                    int i = 0, value;
                    while (inFile >> value && i < 500)
                        memory.Set(i++, std::to_string(value));
                    for (; i < 500; ++i)
                        memory.Set(i, "0");
                    inFile.close();
                } else {
                    show_message("Erro", "Não foi possível abrir " + b.memoryPath);
                }
            }

            // Carrega registradores se necessário
            if (!b.regPath.empty()) {
                inFile.open(b.regPath);
                if (inFile.is_open()) {
                    auto reg_gui = reg.at(0);
                    int i = 0, value;
                    while (inFile >> value && i < 32)
                        reg_gui.at(i++).text(1, std::to_string(value));
                    for (; i < 32; ++i)
                        reg_gui.at(i).text(1, "0");
                    inFile.close();
                } else {
                    show_message("Erro", "Não foi possível abrir " + b.regPath);
                }
            }
        }
    });
    bench_sub.append("Fibonacci",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/fibonacci/fibonacci.txt";
        bench_name = "fibonacci";        
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
        else
            fila = true;
    });
    bench_sub.append("Stall por Divisão",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/division_stall.txt";       
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
        else
            fila = true;
    });
    bench_sub.append("Stress de Memória (Stores)",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/store_stress/store_stress.txt";   
        bench_name = "store_stress";  
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
        else
            fila = true;
    });
    bench_sub.append("Stall por hazard estrutural (Adds)",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/res_stations_stall.txt";       
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
        else
            fila = true;
    }); 
    bench_sub.append("Busca Linear",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/linear_search/linear_search.txt";
        bench_name = "linear_search";
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo");
        else
            fila = true;
        
        path = "in/benchmarks/linear_search/memory.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                int i = 0;
                int value;
                while(inFile >> value && i < 500)
                {
                    memory.Set(i,std::to_string(value));
                    i++;
                }
                for(; i < 500 ; i++)
                {
                    memory.Set(i,"0");
                }
                inFile.close();
            }

        path = "in/benchmarks/linear_search/regi_i.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                auto reg_gui = reg.at(0);
                int value,i = 0;
                while(inFile >> value && i < 32)
                {
                    reg_gui.at(i).text(1,std::to_string(value));
                    i++;
                }
                for(; i < 32 ; i++)
                    reg_gui.at(i).text(1,"0");
                inFile.close();
            }
    });

    bench_sub.append("Busca Binária",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/binary_search/binary_search.txt";
        bench_name = "binary_search";
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo");
        else
            fila = true;
        
        path = "in/benchmarks/binary_search/memory.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                int i = 0;
                int value;
                while(inFile >> value && i < 500)
                {
                    memory.Set(i,std::to_string(value));
                    i++;
                }
                for(; i < 500 ; i++)
                {
                    memory.Set(i,"0");
                }
                inFile.close();
            }

        path = "in/benchmarks/binary_search/regs.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                auto reg_gui = reg.at(0);
                int value,i = 0;
                while(inFile >> value && i < 32)
                {
                    reg_gui.at(i).text(1,std::to_string(value));
                    i++;
                }
                for(; i < 32 ; i++)
                    reg_gui.at(i).text(1,"0");
                inFile.close();
            }
    });

    bench_sub.append("Matriz Search",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/matriz_search/matriz_search.txt";
        bench_name = "matriz_search";
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo");
        else
            fila = true;
        
        path = "in/benchmarks/matriz_search/memory.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                int i = 0;
                int value;
                while(inFile >> value && i < 500)
                {
                    memory.Set(i,std::to_string(value));
                    i++;
                }
                for(; i < 500 ; i++)
                {
                    memory.Set(i,"0");
                }
                inFile.close();
            }

        path = "in/benchmarks/matriz_search/regs.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                auto reg_gui = reg.at(0);
                int value,i = 0;
                while(inFile >> value && i < 32)
                {
                    reg_gui.at(i).text(1,std::to_string(value));
                    i++;
                }
                for(; i < 32 ; i++)
                    reg_gui.at(i).text(1,"0");
                inFile.close();
            }
    });

    bench_sub.append("Bubble Sort",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/bubble_sort/bubble_sort.txt";
        bench_name = "bubble_sort";
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo");
        else
            fila = true;
        
        path = "in/benchmarks/bubble_sort/memory.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                int i = 0;
                int value;
                while(inFile >> value && i < 500)
                {
                    memory.Set(i,std::to_string(value));
                    i++;
                }
                for(; i < 500 ; i++)
                {
                    memory.Set(i,"0");
                }
                inFile.close();
            }

        path = "in/benchmarks/bubble_sort/regs.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                auto reg_gui = reg.at(0);
                int value,i = 0;
                while(inFile >> value && i < 32)
                {
                    reg_gui.at(i).text(1,std::to_string(value));
                    i++;
                }
                for(; i < 32 ; i++)
                    reg_gui.at(i).text(1,"0");
                inFile.close();
            }
    });

    bench_sub.append("Insertion Sort",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/insertion_sort/insertion_sort.txt";
        bench_name = "insertion_sort";
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo");
        else
            fila = true;
        
        path = "in/benchmarks/insertion_sort/memory.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                int i = 0;
                int value;
                while(inFile >> value && i < 500)
                {
                    memory.Set(i,std::to_string(value));
                    i++;
                }
                for(; i < 500 ; i++)
                {
                    memory.Set(i,"0");
                }
                inFile.close();
            }

        path = "in/benchmarks/insertion_sort/regs.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                auto reg_gui = reg.at(0);
                int value,i = 0;
                while(inFile >> value && i < 32)
                {
                    reg_gui.at(i).text(1,std::to_string(value));
                    i++;
                }
                for(; i < 32 ; i++)
                    reg_gui.at(i).text(1,"0");
                inFile.close();
            }
    });

    bench_sub.append("Tick Tack",[&](menu::item_proxy &ip){
        string path = "in/benchmarks/tick_tack/tick_tack.txt";
        bench_name = "tick_tack";
        inFile.open(path);
        if(!add_instructions(inFile,instruction_queue,instruct))
            show_message("Arquivo inválido","Não foi possível abrir o arquivo");
        else
            fila = true;
        
        path = "in/benchmarks/tick_tack/regs.txt";
        inFile.open(path);
            if(!inFile.is_open())
                show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
            else
            {
                auto reg_gui = reg.at(0);
                int value,i = 0;
                while(inFile >> value && i < 32)
                {
                    reg_gui.at(i).text(1,std::to_string(value));
                    i++;
                }
                for(; i < 32 ; i++)
                    reg_gui.at(i).text(1,"0");
                inFile.close();
            }
    });
}
