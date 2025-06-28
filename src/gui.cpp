#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <nana/gui/widgets/label.hpp>
#include <nana/gui.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/button.hpp>
#include "top.hpp"
#include "grid.hpp"

using namespace nana;

const char* str_spec = R"layout(
<vert
    <weight=5%>
    <vert weight=85%
        <
            <weight=1%>
            <instr>
            <weight=1%>
            <rst>
            <weight=1%>
            <weight=20% regs>
            <weight=1%>
        >
        <
            <weight=1%>
            <memor>
            <weight=1%>
            <rob>
            <weight=1%>
        >
    >
    <
        <weight=1%>
        <clk_c weight=10%>
        <weight=30%>
        <gap=10btns>
        <weight=1%>
    >
    <weight=2%>
>
)layout";

const char* str_nospec = R"layout(
<vert
    <weight=5%>
    <vert weight=85%
        <
            <weight=1%>
            <instr>
            <weight=1%>
            <rst>
            <weight=1%>
        >
        <
            <weight=1%>
            <memor>
            <weight=1%>
            <weight=29% regs>
            <weight=1%>
        >
    >
    <
        <weight=1%>
        <clk_c weight=10%>
        <weight=30%>
        <gap=10btns>
        <weight=1%>
    >
    <weight=2%>
>
)layout";

void set_spec(place &plc, bool is_spec)
{
    if(is_spec)
        plc.div(str_spec);
    else
        plc.div(str_nospec);
    plc.collocate();
}

bool add_instructions(ifstream &File, std::vector<std::string> &queue, listbox &instruction_gui)
{
    if(!File.is_open())
        return false;
    if(queue.size())
    {
        queue.clear();
        instruction_gui.clear(0);   
    }
    auto inst_gui_cat = instruction_gui.at(0);
    std::string line;
    while(getline(File,line))
    {
        if(line.rfind("//", 0) == std::string::npos) //ignora linhas que começam com "//"
        {
            queue.push_back(line);
            inst_gui_cat.append(line);
        }
    }
    File.close();
    return true;
}

void show_message(std::string message_title, std::string message)
{
    msgbox msg(message_title);
    msg << message;
    msg.show();
}

void show_metrics_window(form& parent, const std::string& metrics_text) {
    using namespace nana;

    auto metrics_win = new form(API::make_center(400, 300));
    metrics_win->caption("Simulation Metrics");

    label* lbl = new label(*metrics_win);
    lbl->format(true);
    lbl->caption(metrics_text);
    lbl->move({10, 10});
    lbl->size({380, 280});
    lbl->text_align(align::left, align_v::top);

    parent.enabled(false);

    metrics_win->events().unload([&, metrics_win] {
        parent.enabled(true);
        // delete metrics_win;
    });

    metrics_win->show();
    API::modal_window(metrics_win->handle());
    API::refresh_window(metrics_win->handle());
    API::set_window_z_order(metrics_win->handle(), nullptr, z_order_action::top);
}

// Setup dos Headers

void setup_register_headers(listbox& reg) {
    std::vector<std::string> columns = {"", "Value", "Qi"};
    std::vector<int> sizes = {30, 75, 40};

    for (int k = 0; k < 2; ++k) {
        for (size_t i = 0; i < columns.size(); ++i) {
            reg.append_header(columns[i]);
            reg.column_at(k * columns.size() + i).width(sizes[i]);
        }
    }

    auto reg_gui = reg.at(0);
    for (int i = 0; i < 32; ++i) {
        std::string index = to_string(i);
        reg_gui.append("R" + index);
        reg_gui.at(i).text(3, "F" + index);
    }
}

void setup_table_headers(listbox& table) {
    std::vector<std::string> columns = {"#", "Name", "Busy", "Op", "Vj", "Vk", "Qj", "Qk", "A"};
    for (size_t i = 0; i < columns.size(); ++i) {
        table.append_header(columns[i]);
        if (i == 0)
            table.column_at(i).width(30);
        else if (i == 3)
            table.column_at(i).width(60);
        else
            table.column_at(i).width(45);
    }
}

void setup_instruction_headers(listbox& instruct) {
    std::vector<std::string> columns = {"Instruction", "Issue", "Execute", "Write Result"};
    std::vector<int> sizes = {140, 60, 70, 95};

    for (size_t i = 0; i < columns.size(); ++i) {
        instruct.append_header(columns[i]);
        instruct.column_at(i).width(sizes[i]);
    }
}

void setup_rob_headers(listbox& rob) {
    std::vector<std::string> columns = {"Entry", "Busy", "Instruction", "State", "Destination", "Value"};
    std::vector<int> sizes = {45, 45, 120, 120, 90, 60};

    for (size_t i = 0; i < columns.size(); ++i) {
        rob.append_header(columns[i]);
        rob.column_at(i).width(sizes[i]);
    }
}

// Setup dos dados

void fill_registers(listbox& reg) {
    auto reg_gui = reg.at(0);
    srand(static_cast<unsigned>(time(nullptr)));

    for (int i = 0; i < 32; ++i) {
        if (i == 0)
            reg_gui.at(i).text(1, "0");
        else
            reg_gui.at(i).text(1, to_string(rand() % 100));

        reg_gui.at(i).text(2, "0");

        ostringstream ss;
        ss << fixed << setprecision(2)
           << static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 100.0);
        reg_gui.at(i).text(4, ss.str());
        reg_gui.at(i).text(5, "0");
    }
}

void fill_memory(grid& memory) {
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < 500; ++i)
        memory.Push(to_string(rand() % 100));
}

void parse_arguments(
    int argc, char* argv[],
    std::vector<std::string>& instruction_queue,
    listbox& instruct,
    listbox& reg,
    grid& memory,
    std::map<std::string,int>& instruct_time,
    int& nadd, int& nmul, int& nls,
    bool& fila,
    form& fm
) {
    using namespace nana;
    ifstream inFile;

    for (int k = 1; k < argc; k += 2) {
        if (strlen(argv[k]) > 2) {
            show_message("Opção inválida", std::string("Opção \"") + argv[k] + "\" inválida");
            continue;
        }

        char c = argv[k][1];
        int i = 0;

        switch(c) {
            case 'q':
                inFile.open(argv[k+1]);
                if (!add_instructions(inFile, instruction_queue, instruct))
                    show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
                else
                    fila = true;
                break;
            case 'i':
                inFile.open(argv[k+1]);
                i = 0;
                if (!inFile.is_open())
                    show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
                else {
                    int value;
                    auto reg_gui = reg.at(0);
                    while (inFile >> value && i < 32)
                        reg_gui.at(i++).text(1, to_string(value));
                    for (; i < 32; ++i)
                        reg_gui.at(i).text(1, "0");
                    inFile.close();
                }
                break;
            case 'f':
                inFile.open(argv[k+1]);
                i = 0;
                if (!inFile.is_open())
                    show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
                else {
                    float value_fp;
                    auto reg_gui = reg.at(0);
                    while (inFile >> value_fp && i < 32)
                        reg_gui.at(i++).text(4, to_string(value_fp));
                    for (; i < 32; ++i)
                        reg_gui.at(i).text(4, "0");
                    inFile.close();
                }
                break;
            case 'm':
                inFile.open(argv[k+1]);
                if (!inFile.is_open())
                    show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
                else {
                    int value; i = 0;
                    while (inFile >> value && i < 500)
                        memory.Set(i++, to_string(value));
                    for (; i < 500; ++i)
                        memory.Set(i, "0");
                    inFile.close();
                }
                break;
            case 'r':
                inFile.open(argv[k+1]);
                if (!inFile.is_open())
                    show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
                else {
                    int value;
                    if (inFile >> value && value <= 10 && value > 0) nadd = value;
                    if (inFile >> value && value <= 10 && value > 0) nmul = value;
                    if (inFile >> value && value <= 10 && value > 0) nls = value;
                    inFile.close();
                }
                break;
            case 's':
                // Esta flag não tem argumento, decrementa para compensar
                fila = true;
                k--;
                break;
            case 'l':
                inFile.open(argv[k+1]);
                if (!inFile.is_open())
                    show_message("Arquivo inválido","Não foi possível abrir o arquivo!");
                else {
                    std::string inst;
                    int value;
                    while (inFile >> inst) {
                        if (inFile >> value && instruct_time.count(inst))
                            instruct_time[inst] = value;
                    }
                    inFile.close();
                }
                break;
            default:
                show_message("Opção inválida", std::string("Opção \"") + argv[k] + "\" inválida");
                break;
        }
    }
}
