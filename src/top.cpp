#include "top.hpp"

top::top(sc_module_name name): sc_module(name){}

void top::simple_mode(unsigned int nadd, unsigned int nmul,unsigned int nload,map<string,int> instruct_time,vector<string> instruct_queue, nana::listbox &table, nana::grid &mem_gui, nana::listbox &regs, nana::listbox &instr_gui, nana::label &ccount)
{
    CDB = unique_ptr<bus>(new bus("CDB"));
    mem_bus = unique_ptr<bus>(new bus("mem_bus"));
    clock_bus = unique_ptr<bus>(new bus("clock_bus"));
    inst_bus = unique_ptr<cons_bus>(new cons_bus("inst_bus"));
    rst_bus = unique_ptr<cons_bus>(new cons_bus("rst_bus"));
    sl_bus = unique_ptr<cons_bus>(new cons_bus("sl_bus"));
    rb_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rb_bus"));

    iss_ctrl = unique_ptr<issue_control>(new issue_control("issue_control"));
    clk = unique_ptr<clock_>(new clock_("clock",1,ccount));
    fila = unique_ptr<instruction_queue>(new instruction_queue("fila_inst",instruct_queue,instr_gui));
    rs_ctrl = unique_ptr<res_vector>(new res_vector("rs_control",nadd,nmul,instruct_time,table,instr_gui.at(0)));
    rb = unique_ptr<register_bank>(new register_bank("register_bank", regs));
    slb = unique_ptr<sl_buffer>(new sl_buffer("sl_buffer_control",nload,nadd+nmul,instruct_time,table,instr_gui.at(0)));
    mem = unique_ptr<memory>(new memory("memoria", mem_gui));

    clk->out(*clock_bus);

    fila->in(*clock_bus);
    fila->out(*inst_bus);

    iss_ctrl->in(*inst_bus);
    iss_ctrl->out_rsv(*rst_bus);
    iss_ctrl->out_slbuff(*sl_bus);

    rs_ctrl->in_issue(*rst_bus);
    rs_ctrl->in_cdb(*CDB);
    rs_ctrl->out_cdb(*CDB);
    rs_ctrl->in_rb(*rb_bus);
    rs_ctrl->out_rb(*rb_bus);
    rs_ctrl->out_mem(*mem_bus);

    slb->in_issue(*sl_bus);
    slb->in_cdb(*CDB);
    slb->out_cdb(*CDB);
    slb->in_rb(*rb_bus);
    slb->out_rb(*rb_bus);
    slb->out_mem(*mem_bus);

    rb->in(*rb_bus);
    rb->out(*rb_bus);       
    rb->in_cdb(*CDB);

    mem->in(*mem_bus);
    mem->out(*CDB);
}

void top::rob_mode(int n_bits, unsigned int nadd, unsigned int nmul,unsigned int nload,map<string,int> instruct_time, vector<string> instruct_queue, nana::listbox &table, nana::grid &mem_gui, nana::listbox &regs, nana::listbox &instr_gui, nana::label &ccount, nana::listbox &rob_gui)
{
    int rob_size = 10; //Tamanho do REORDER BUFFER
    CDB = unique_ptr<bus>(new bus("CDB"));
    mem_bus = unique_ptr<bus>(new bus("mem_bus"));
    clock_bus = unique_ptr<bus>(new bus("clock_bus"));
    adu_bus = unique_ptr<bus>(new bus("adu_bus"));
    adu_sl_bus = unique_ptr<bus>(new bus("adu_sl_bus"));
    mem_slb_bus = unique_ptr<bus>(new bus("mem_slb_bus"));
    iq_rob_bus = unique_ptr<bus>(new bus("iq_rob_bus"));
    rob_statval_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rob_statval_bus"));//Este canal se comunida com rs_ctrl_r e com adu
    rob_adu_bus = unique_ptr<cons_bus>(new cons_bus("rob_adu_bus")); //usado para flush
    inst_bus = unique_ptr<cons_bus>(new cons_bus("inst_bus"));
    rst_bus = unique_ptr<cons_bus>(new cons_bus("rst_bus"));
    sl_bus = unique_ptr<cons_bus>(new cons_bus("sl_bus"));
    rob_bus = unique_ptr<cons_bus>(new cons_bus("rob_bus"));
    ad_bus = unique_ptr<cons_bus>(new cons_bus("ad_bus"));
    rob_slb_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rob_slb_bus"));
    rb_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rb_bus"));

    clk = unique_ptr<clock_>(new clock_("clock",1,ccount));
    iss_ctrl_r = unique_ptr<issue_control_rob>(new issue_control_rob("issue_control_rob"));
    fila_r = unique_ptr<instruction_queue_rob>(new instruction_queue_rob("fila_inst_rob",instruct_queue,rob_size,instr_gui));
    rob = unique_ptr<reorder_buffer>(new reorder_buffer("rob",rob_size,n_bits, 0, 1, rob_gui,instr_gui.at(0)));
    adu = unique_ptr<address_unit>(new address_unit("address_unit",instruct_time["MEM"],instr_gui.at(0),table.at(0),nadd+nmul));
    rs_ctrl_r = unique_ptr<res_vector_rob>(new res_vector_rob("rs_vc",nadd,nmul,instruct_time,table,instr_gui.at(0),rob_gui.at(0)));
    rb_r = unique_ptr<register_bank_rob>(new register_bank_rob("register_bank_rob",regs));
    slb_r = unique_ptr<sl_buffer_rob>(new sl_buffer_rob("sl_buffer_rob",nload,nadd+nmul,instruct_time,table,instr_gui.at(0),rob_gui.at(0)));
    mem_r = unique_ptr<memory_rob>(new memory_rob("memory_rob", mem_gui));

    clk->out(*clock_bus);

    fila_r->in(*clock_bus);
    fila_r->out(*inst_bus);
    fila_r->in_rob(*iq_rob_bus);

    iss_ctrl_r->in(*inst_bus);
    iss_ctrl_r->out_rsv(*rst_bus);
    iss_ctrl_r->out_slbuff(*sl_bus);
    iss_ctrl_r->in_slbuff(*sl_bus);
    iss_ctrl_r->in_rob(*rob_bus);
    iss_ctrl_r->out_rob(*rob_bus);
    iss_ctrl_r->out_adu(*ad_bus);

    rob->in_issue(*rob_bus);
    rob->out_issue(*rob_bus);
    rob->in_cdb(*CDB);
    rob->in_rb(*rb_bus);
    rob->out_rb(*rb_bus);
    rob->out_mem(*mem_bus);
    rob->in_adu(*adu_bus);
    rob->out_iq(*iq_rob_bus);
    rob->in_resv_adu(*rob_statval_bus);
    rob->out_resv_adu(*rob_statval_bus);
    rob->in_slb(*rob_slb_bus);
    rob->out_slb(*rob_slb_bus);
    rob->out_adu(*rob_adu_bus);

    adu->in_issue(*ad_bus);
    adu->in_cdb(*CDB);
    adu->out_slbuff(*adu_sl_bus);
    adu->in_rob(*rob_adu_bus);
    adu->out_rob(*adu_bus);
    adu->in_rb(*rb_bus);
    adu->out_rb(*rb_bus);
    adu->in_rob_svl(*rob_statval_bus);
    adu->out_rob_svl(*rob_statval_bus);

    rs_ctrl_r->in_issue(*rst_bus);
    rs_ctrl_r->in_cdb(*CDB);
    rs_ctrl_r->out_cdb(*CDB);
    rs_ctrl_r->in_rb(*rb_bus);
    rs_ctrl_r->out_rb(*rb_bus);
    rs_ctrl_r->out_mem(*mem_bus);
    rs_ctrl_r->in_rob(*rob_statval_bus);
    rs_ctrl_r->out_rob(*rob_statval_bus);

    slb_r->in_issue(*sl_bus);
    slb_r->out_issue(*sl_bus);
    slb_r->in_cdb(*CDB);
    slb_r->out_cdb(*CDB);
    slb_r->out_mem(*mem_bus);
    slb_r->in_adu(*adu_sl_bus);
    slb_r->in_mem(*mem_slb_bus);
    slb_r->in_rob(*rob_slb_bus);
    slb_r->out_rob(*rob_slb_bus);

    rb_r->in(*rb_bus);
    rb_r->out(*rb_bus);

    mem_r->in(*mem_bus);
    mem_r->out(*CDB);
    mem_r->out_slb(*mem_slb_bus);
}

void top::rob_mode_bpb(int n_bits, int bpb_size, unsigned int nadd, unsigned int nmul,unsigned int nload,map<string,int> instruct_time, vector<string> instruct_queue, nana::listbox &table, nana::grid &mem_gui, nana::listbox &regs, nana::listbox &instr_gui, nana::label &ccount, nana::listbox &rob_gui)
{
    int rob_size = 10; //Tamanho do REORDER BUFFER
    CDB = unique_ptr<bus>(new bus("CDB"));
    mem_bus = unique_ptr<bus>(new bus("mem_bus"));
    clock_bus = unique_ptr<bus>(new bus("clock_bus"));
    adu_bus = unique_ptr<bus>(new bus("adu_bus"));
    adu_sl_bus = unique_ptr<bus>(new bus("adu_sl_bus"));
    mem_slb_bus = unique_ptr<bus>(new bus("mem_slb_bus"));
    iq_rob_bus = unique_ptr<bus>(new bus("iq_rob_bus"));
    rob_statval_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rob_statval_bus"));//Este canal se comunida com rs_ctrl_r e com adu
    rob_adu_bus = unique_ptr<cons_bus>(new cons_bus("rob_adu_bus")); //usado para flush
    inst_bus = unique_ptr<cons_bus>(new cons_bus("inst_bus"));
    rst_bus = unique_ptr<cons_bus>(new cons_bus("rst_bus"));
    sl_bus = unique_ptr<cons_bus>(new cons_bus("sl_bus"));
    rob_bus = unique_ptr<cons_bus>(new cons_bus("rob_bus"));
    ad_bus = unique_ptr<cons_bus>(new cons_bus("ad_bus"));
    rob_slb_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rob_slb_bus"));
    rb_bus = unique_ptr<cons_bus_fast>(new cons_bus_fast("rb_bus"));

    clk = unique_ptr<clock_>(new clock_("clock",1,ccount));
    iss_ctrl_r = unique_ptr<issue_control_rob>(new issue_control_rob("issue_control_rob"));
    fila_r = unique_ptr<instruction_queue_rob>(new instruction_queue_rob("fila_inst_rob",instruct_queue,rob_size,instr_gui));
    rob = unique_ptr<reorder_buffer>(new reorder_buffer("rob",rob_size, n_bits, bpb_size, 2, rob_gui,instr_gui.at(0)));
    adu = unique_ptr<address_unit>(new address_unit("address_unit",instruct_time["MEM"],instr_gui.at(0),table.at(0),nadd+nmul));
    rs_ctrl_r = unique_ptr<res_vector_rob>(new res_vector_rob("rs_vc",nadd,nmul,instruct_time,table,instr_gui.at(0),rob_gui.at(0)));
    rb_r = unique_ptr<register_bank_rob>(new register_bank_rob("register_bank_rob",regs));
    slb_r = unique_ptr<sl_buffer_rob>(new sl_buffer_rob("sl_buffer_rob",nload,nadd+nmul,instruct_time,table,instr_gui.at(0),rob_gui.at(0)));
    mem_r = unique_ptr<memory_rob>(new memory_rob("memory_rob", mem_gui));

    clk->out(*clock_bus);

    fila_r->in(*clock_bus);
    fila_r->out(*inst_bus);
    fila_r->in_rob(*iq_rob_bus);

    iss_ctrl_r->in(*inst_bus);
    iss_ctrl_r->out_rsv(*rst_bus);
    iss_ctrl_r->out_slbuff(*sl_bus);
    iss_ctrl_r->in_slbuff(*sl_bus);
    iss_ctrl_r->in_rob(*rob_bus);
    iss_ctrl_r->out_rob(*rob_bus);
    iss_ctrl_r->out_adu(*ad_bus);

    rob->in_issue(*rob_bus);
    rob->out_issue(*rob_bus);
    rob->in_cdb(*CDB);
    rob->in_rb(*rb_bus);
    rob->out_rb(*rb_bus);
    rob->out_mem(*mem_bus);
    rob->in_adu(*adu_bus);
    rob->out_iq(*iq_rob_bus);
    rob->in_resv_adu(*rob_statval_bus);
    rob->out_resv_adu(*rob_statval_bus);
    rob->in_slb(*rob_slb_bus);
    rob->out_slb(*rob_slb_bus);
    rob->out_adu(*rob_adu_bus);

    adu->in_issue(*ad_bus);
    adu->in_cdb(*CDB);
    adu->out_slbuff(*adu_sl_bus);
    adu->in_rob(*rob_adu_bus);
    adu->out_rob(*adu_bus);
    adu->in_rb(*rb_bus);
    adu->out_rb(*rb_bus);
    adu->in_rob_svl(*rob_statval_bus);
    adu->out_rob_svl(*rob_statval_bus);

    rs_ctrl_r->in_issue(*rst_bus);
    rs_ctrl_r->in_cdb(*CDB);
    rs_ctrl_r->out_cdb(*CDB);
    rs_ctrl_r->in_rb(*rb_bus);
    rs_ctrl_r->out_rb(*rb_bus);
    rs_ctrl_r->out_mem(*mem_bus);
    rs_ctrl_r->in_rob(*rob_statval_bus);
    rs_ctrl_r->out_rob(*rob_statval_bus);

    slb_r->in_issue(*sl_bus);
    slb_r->out_issue(*sl_bus);
    slb_r->in_cdb(*CDB);
    slb_r->out_cdb(*CDB);
    slb_r->out_mem(*mem_bus);
    slb_r->in_adu(*adu_sl_bus);
    slb_r->in_mem(*mem_slb_bus);
    slb_r->in_rob(*rob_slb_bus);
    slb_r->out_rob(*rob_slb_bus);

    rb_r->in(*rb_bus);
    rb_r->out(*rb_bus);

    mem_r->in(*mem_bus);
    mem_r->out(*CDB);
    mem_r->out_slb(*mem_slb_bus);
}

MetricsResult top::compute_metrics(int cpu_freq, int mode, int n_bits) {
    MetricsResult result{};
    result.cpu_freq = cpu_freq;
    result.tam_bpb = 0;
    result.mem_count = 0;

    double tempo_ciclo_clock = 1.0 / (cpu_freq * 1e6);
    double tempo_ciclo_clock_ns = tempo_ciclo_clock * 1e9;
    result.ciclos = (sc_time_stamp().to_double() / 1000.0) - 1;

    if ((mode == 1 || mode == 2) && fila_r && rob) {
        result.total_instructions_exec = fila_r->get_instruction_counter();
        result.mem_count = rob->get_mem_count();
    } else if (mode == 0 && fila) {
        result.total_instructions_exec = fila->get_instruction_counter();
    }

    if (result.total_instructions_exec > 0) {
        result.cpi_medio = result.ciclos / result.total_instructions_exec;
        result.t_cpu = result.cpi_medio * result.total_instructions_exec * tempo_ciclo_clock_ns;
        result.mips = result.total_instructions_exec / (result.t_cpu * 1e-9 * 1e6);

        if (mode == 1) {
            result.hit_rate = get_rob().get_preditor().get_predictor_hit_rate();
        } else if (mode == 2) {
            result.hit_rate = get_rob().get_bpb().bpb_get_hit_rate();
            result.tam_bpb = get_rob().get_bpb().get_bpb_size();
        }
    }

    return result;
}

void top::metrics(int cpu_freq, int mode, string bench_name, int n_bits) {
    MetricsResult result = compute_metrics(cpu_freq, mode, n_bits);

    if (result.total_instructions_exec == 0) {
        std::cerr << "[Aviso] Nenhuma instrução foi executada.\n";
        return;
    }

    std::cout << "\n\nMÉTRICAS:\n"
              << "# Frequência CPU: " << result.cpu_freq << " MHz\n"
              << "# Total de Instruções Executadas: " << result.total_instructions_exec << "\n"
              << "# Ciclos: " << result.ciclos << "\n"
              << "# CPI Médio: " << result.cpi_medio << "\n"
              << "# t_CPU: " << result.t_cpu << " ns\n"
              << "# MIPS: " << result.mips << " milhões de instruções por segundo\n"
              << "# Acessos à memória: " << result.mem_count << "\n"
              << "# Preditor: " << n_bits << " bits\n";

    if (mode == 1) {
        std::cout << "# Taxa de sucesso - 1 Preditor: " << result.hit_rate << "%\n";
    } else if (mode == 2) {
        std::cout << "# Taxa de sucesso - BPB[" << result.tam_bpb << "]: " << result.hit_rate << "%\n";
    }

    dump_metrics_csv(bench_name, result);
}

std::string top::get_metrics_text(int cpu_freq, int mode, const std::string& bench_name, int n_bits) {
    MetricsResult result = compute_metrics(cpu_freq, mode, n_bits);
    std::ostringstream out;

    if (result.total_instructions_exec == 0)
        return "[Aviso] Nenhuma instrução foi executada.\n";

    out << "Frequência CPU: " << result.cpu_freq << " MHz\n"
        << "Instruções Executadas: " << result.total_instructions_exec << "\n"
        << "Ciclos: " << result.ciclos << "\n"
        << "CPI Médio: " << result.cpi_medio << "\n"
        << "Tempo CPU (t_CPU): " << result.t_cpu << " ns\n"
        << "MIPS: " << result.mips << " milhões de instruções/s\n"
        << "Acessos à Memória: " << result.mem_count << "\n"
        << "Bits do Preditor: " << n_bits << "\n";

    if (mode == 1) {
        out << "Taxa de Acerto (1 Preditor): " << result.hit_rate << "%\n";
    } else if (mode == 2) {
        out << "Taxa de Acerto (BPB[" << result.tam_bpb << "]): " << result.hit_rate << "%\n";
    }

    return out.str();
}

void top::dump_metrics(std::string bench_name, MetricsResult result) {
    using std::endl;
    const std::filesystem::path benchmark{"./in/benchmarks/" + bench_name};

    std::ofstream out_file{benchmark / "metrics.txt", std::ios_base::app};

    out_file << "MÉTRICAS:\n"
             << "# Frequência CPU: " << result.cpu_freq << " MHz\n"
             << "# Total de Instruções Executadas: " << result.total_instructions_exec << "\n"
             << "# Ciclos: " << result.ciclos << "\n"
             << "# CPI Médio: " << result.cpi_medio << "\n"
             << "# t_CPU: " << result.t_cpu << " ns\n"
             << "# MIPS: " << result.mips << " milhões de instruções por segundo\n"
             << "# Acessos à memória: " << result.mem_count << "\n"
             << "# Preditor: " << result.tam_bpb << " bits\n";

    if (result.tam_bpb > 0)
        out_file << "# Taxa de sucesso - BPB[" << result.tam_bpb << "]: " << result.hit_rate << "%\n";
    else
        out_file << "# Taxa de sucesso - 1 Preditor: " << result.hit_rate << "%\n";

    out_file.close();
}

void top::dump_metrics_csv(std::string bench_name, MetricsResult result) {
    const std::filesystem::path benchmark_dir{"./in/benchmarks/" + bench_name};
    std::filesystem::create_directories(benchmark_dir);

    std::ofstream out_file(benchmark_dir / "metrics.csv", std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "[Erro] Não foi possível abrir metrics.csv para escrita.\n";
        return;
    }

    // Se arquivo vazio, escreve header
    out_file.seekp(0, std::ios::end);
    if (out_file.tellp() == 0) {
        out_file << "Benchmark,Frequência_CPU_MHz,Instruções_Executadas,Ciclos,CPI_Médio,t_CPU_ns,MIPS,Acessos_memória,Preditor_bits,Taxa_sucesso\n";
    }

    out_file << bench_name << ","
             << result.cpu_freq << ","
             << result.total_instructions_exec << ","
             << result.ciclos << ","
             << result.cpi_medio << ","
             << result.t_cpu << ","
             << result.mips << ","
             << result.mem_count << ","
             << result.tam_bpb << ","
             << result.hit_rate << "\n";

    out_file.close();
}
