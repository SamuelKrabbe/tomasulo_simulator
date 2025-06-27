#include<string>
#include<vector>
#include<map>
#include<fstream>
#include<nana/gui.hpp>
#include "gui.hpp"
#include "top.hpp"

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

// Mostra mensagem na interface grafica
void show_message(string message_title, string message)
{
    msgbox msg(message_title);
    msg << message;
    msg.show();
}
// Organiza a interface de acordo com o modo (com ou sem especulacao)
void set_spec(nana::place &plc, bool is_spec)
{
    if(is_spec)
        plc.div(str_spec);
    else
        plc.div(str_nospec);
    plc.collocate();
}


bool add_instructions(ifstream &File,vector<string> &queue, nana::listbox &instruction_gui)
{
    if(!File.is_open())
        return false;
    if(queue.size())
    {
        queue.clear();
        instruction_gui.clear(0);   
    }
    auto inst_gui_cat = instruction_gui.at(0);
    string line;
    while(getline(File,line))
    {
        if(line.rfind("//", 0) == string::npos) //ignora linhas que começam com "//"
        {
            queue.push_back(line);
            inst_gui_cat.append(line);
        }
    }
    File.close();
    return true;
}

void show_metrics_window(nana::form& parent, const std::string& metrics_text) {
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
    nana::API::refresh_window(metrics_win->handle());
    nana::API::set_window_z_order(metrics_win->handle(), nullptr, nana::z_order_action::top);
}
