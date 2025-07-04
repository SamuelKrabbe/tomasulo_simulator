#include<systemc.h>
#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<nana/gui.hpp>
#include<nana/gui/widgets/button.hpp>
#include<nana/gui/widgets/menubar.hpp>
#include<nana/gui/widgets/group.hpp>
#include<nana/gui/widgets/textbox.hpp>
#include<nana/gui/filebox.hpp>
#include "menu_handlers.hpp"
#include "top.hpp"
#include "gui.hpp"

using std::string;
using std::vector;
using std::map;
using std::fstream;

int sc_main(int argc, char *argv[])
{
    using namespace nana;

    vector<string> instruction_queue;
    string bench_name = "";
    int nadd = 3, nmul = 2, nls = 2, n_bits = 2, bpb_size = 4, cpu_freq = 500;
    vector<int> sizes;
    bool spec = false;
    int mode = 0;
    bool fila = false;
    ifstream inFile;

    form fm(API::make_center(1024,700));
    place plc(fm);
    place upper(fm);
    place lower(fm);

    // Painéis e componentes
    panel<true> panel_instr(fm), panel_rst(fm), panel_regs(fm), panel_rob(fm), panel_memor(fm);
    listbox table(panel_rst), reg(panel_regs), instruct(panel_instr), rob(panel_rob);
    grid memory(panel_memor, rectangle(), 10, 50);

    menubar mnbar(fm);
    button metrics(fm), start(fm), run_all(fm), clock_control(fm), exit(fm);
    group clock_group(fm);
    label clock_count(clock_group);

    fm.caption("TFSim");
    clock_group.caption("Ciclo");
    clock_group.div("count");

    map<string,int> instruct_time{
        {"DADD",4},{"DADDI",4},{"DSUB",6},{"DSUBI",6},
        {"DMUL",10},{"DDIV",16},{"MEM",2},{"SLT",1},{"SGT",1}
    };
    
    // Simulador
    top top1("top");

    // Configura labels e botões
    metrics.caption("Metrics");
    start.caption("Start");
    clock_control.caption("Next cycle");
    run_all.caption("Run all");
    exit.caption("Exit");

    // Layout de menu, botões e painéis
    plc["btns"]   << metrics << start << clock_control << run_all << exit;
    plc["rst"]    << panel_rst;
    plc["memor"]  << panel_memor;
    plc["regs"]   << panel_regs;
    plc["rob"]    << panel_rob;
    plc["instr"]  << panel_instr;
    plc["clk_c"]  << clock_group;

    clock_group["count"] << clock_count;
    clock_group.collocate();

    // Layout interno com margem para cada painel
    place plc_instr(panel_instr);
    plc_instr.div("<margin=[0,0,10,0] instr>");
    plc_instr["instr"] << instruct;
    plc_instr.collocate();

    place plc_rst(panel_rst);
    plc_rst.div("<margin=[0,0,10,0] rst>");
    plc_rst["rst"] << table;
    plc_rst.collocate();

    place plc_regs(panel_regs);
    plc_regs.div("<margin=[0,0,10,0] regs>");
    plc_regs["regs"] << reg;
    plc_regs.collocate();

    place plc_rob(panel_rob);
    plc_rob.div("<margin=[0,0,10,0] rob>");
    plc_rob["rob"] << rob;
    plc_rob.collocate();

    place plc_mem(panel_memor);
    plc_mem.div("<margin=[0,0,10,0] memor>");
    plc_mem["memor"] << memory;
    plc_mem.collocate();

    //set_spec eh so visual
    set_spec(plc,spec);
    plc.collocate();

    // Setup headers e preenchimento
    setup_table_headers(table);
    setup_register_headers(reg);
    setup_instruction_headers(instruct);
    setup_rob_headers(rob);
    fill_registers(reg);
    fill_memory(memory);

    // Menu
    mnbar.push_back("Opções");
    menu& options_menu = mnbar.at(0);

    options_menu.append("Especulação");
    auto spec_sub = options_menu.create_sub_menu(0);
    
    options_menu.append("Modificar valores...");
    auto config_sub = options_menu.create_sub_menu(1);
    
    options_menu.append("Verificar conteúdo...");
    auto verify_sub = options_menu.create_sub_menu(2);
    
    options_menu.append("Benchmarks");
    auto bench_sub = options_menu.create_sub_menu(3);

    setup_main_menu(
        options_menu, spec, mode, plc, fm, bpb_size, n_bits, nadd, nmul, nls, cpu_freq,
        instruct_time, instruction_queue, inFile, fila,
        instruct, reg, memory, bench_name,
        spec_sub, config_sub, verify_sub, bench_sub, const_cast<const char**>(argv)
    );

    // Argumentos da linha de comando
    parse_arguments(argc, argv, instruction_queue, instruct, reg, memory, instruct_time, nadd, nmul, nls, fila, fm);

    // Eventos
    metrics.enabled(false);
    clock_control.enabled(false);
    run_all.enabled(false);

    metrics.events().click([&] {
        metrics.enabled(false);
        show_metrics_window(fm);
        metrics.enabled(true);
    });
    
    start.events().click([&] {
        if (fila) {
            start.enabled(false);
            clock_control.enabled(true);
            run_all.enabled(true);

            options_menu.enabled(0, false);
            options_menu.enabled(1, false);
            options_menu.enabled(3, false);

            if (spec_sub && spec_sub->size() > 0) {
                for (std::size_t i = 0; i < spec_sub->size(); ++i)
                    spec_sub->enabled(i, false);
            }
            if (config_sub && config_sub->size() > 0) {
                for (std::size_t i = 0; i < config_sub->size(); ++i)
                    config_sub->enabled(i, false);
            }
            if (bench_sub && bench_sub->size() > 0) {
                for (std::size_t i = 0; i < bench_sub->size(); ++i)
                    bench_sub->enabled(i, false);
            }

            if (spec) {
                if (mode == 1)
                    top1.rob_mode(n_bits, nadd, nmul, nls, instruct_time, instruction_queue,
                                  table, memory, reg, instruct, clock_count, rob);
                else if (mode == 2)
                    top1.rob_mode_bpb(n_bits, bpb_size, nadd, nmul, nls, instruct_time, instruction_queue,
                                      table, memory, reg, instruct, clock_count, rob);
            } else {
                top1.simple_mode(nadd, nmul, nls, instruct_time, instruction_queue,
                                 table, memory, reg, instruct, clock_count);
            }
            sc_start();
        } else {
            show_message("Fila de instruções vazia", "A fila de instruções está vazia. Insira um conjunto de instruções para iniciar.");
        }
    });

    clock_control.events().click([&] {
        bool queue_empty = true;
        bool rob_empty = true;

        if (spec) {
            queue_empty = top1.get_rob_queue().queue_is_empty();
            rob_empty = top1.get_rob().rob_is_empty();
        } else {
            queue_empty = top1.get_queue().queue_is_empty();
        }

        if (queue_empty && rob_empty) {
            top1.metrics(cpu_freq, mode, bench_name, n_bits);
            metrics.enabled(true);
        }

        if (sc_is_running()) sc_start();
    });

    run_all.events().click([&] {
        if (spec) {
            while (!(top1.get_rob_queue().queue_is_empty() && top1.get_rob().rob_is_empty())) {
                if (sc_is_running()) sc_start();
            }
        } else {
            while (!top1.get_queue().queue_is_empty()) {
                if (sc_is_running()) sc_start();
            }
        }

        top1.metrics(cpu_freq, mode, bench_name, n_bits);
        metrics.enabled(true);
    });

    exit.events().click([] {
        sc_stop();
        API::exit();
    });

    fm.show();
    exec();
    return 0;
}
