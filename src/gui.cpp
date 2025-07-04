#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <cmath>

#include <nana/gui/widgets/label.hpp>
#include <nana/gui.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/gui/msgbox.hpp>
#include <nana/paint/graphics.hpp>
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

std::string format_large_number(double value) {
    std::ostringstream oss;
    if (std::abs(value) >= 1e9) { // Bilhões
        oss << std::fixed << std::setprecision(2) << value / 1e9 << " B";
    } else if (std::abs(value) >= 1e6) { // Milhões
        oss << std::fixed << std::setprecision(2) << value / 1e6 << " M";
    } else if (std::abs(value) >= 1e3) { // Milhares
        oss << std::fixed << std::setprecision(2) << value / 1e3 << " K";
    } else {
        if (value != 0 && std::abs(value) < 1.0) {
             oss << std::fixed << std::setprecision(2) << value;
        } else {
             oss << std::fixed << std::setprecision(0) << value;
        }
    }
    return oss.str();
}

void show_metrics_window(nana::form& parent) {
    using namespace nana;

    // Janela principal da métrica
    auto metrics_win = new form(API::make_center(950, 550));
    metrics_win->caption("Benchmark Metrics Chart");

    // Combo box para escolher métrica
    combox metric_selector{*metrics_win, rectangle(750, 30, 180, 25)};

    // Métricas disponíveis
    std::vector<std::string> metric_names = {
        "CPI_Médio", "MIPS", "Instruções_Executadas", "Ciclos", "Taxa_sucesso"
    };

    for (const auto& m : metric_names)
        metric_selector.push_back(m);
    metric_selector.option(0); // Seleciona "CPI_Médio" por padrão

    // Estrutura para dados: nome benchmark + mapa <métrica, valor>
    std::vector<std::pair<std::string, std::map<std::string, double>>> benchmarks_data;

    // Leitura dos benchmarks
    for (const auto& entry : std::filesystem::directory_iterator("./in/benchmarks")) {
        if (!entry.is_directory()) continue;

        std::ifstream file(entry.path() / "metrics.csv");
        if (!file.is_open()) {
            std::cerr << "Erro: Nao foi possivel abrir o arquivo metrics.csv em " << entry.path() << std::endl;
            continue;
        }

        std::string header_line;
        std::getline(file, header_line);
        std::stringstream header_ss(header_line);
        std::vector<std::string> headers;
        std::string token;
        while (std::getline(header_ss, token, ',')) {
            headers.push_back(token);
        }

        std::string data_line;
        if (std::getline(file, data_line)) {
            std::stringstream data_ss(data_line);
            std::vector<std::string> values;
            while (std::getline(data_ss, token, ',')) {
                values.push_back(token);
            }

            if (values.empty()) {
                std::cerr << "Aviso: Nenhuma linha de dados em " << entry.path() << "/metrics.csv" << std::endl;
                continue;
            }
            std::string benchmark = values[0];
            std::map<std::string, double> metrics;

            for (const auto& metric : metric_names) {
                auto it = std::find(headers.begin(), headers.end(), metric);
                if (it != headers.end()) {
                    int idx = std::distance(headers.begin(), it);
                    if (idx < (int)values.size()) {
                        try {
                            double val = std::stod(values[idx]);
                            // Verificamos se o valor é finito e não é NaN (Not a Number)
                            // Além disso, filtramos valores negativos, pois métricas de benchmark geralmente são positivas.
                            if (std::isfinite(val) && val >= 0.0)
                                metrics[metric] = val;
                            else {
                                std::cerr << "Aviso: Valor invalido para a metrica '" << metric << "' no benchmark '"
                                          << benchmark << "': '" << values[idx] << "'. Ignorando." << std::endl;
                            }
                        }
                        catch (const std::invalid_argument& e) {
                            std::cerr << "Aviso: Formato de numero invalido para a metrica '" << metric << "' no benchmark '"
                                      << benchmark << "': '" << values[idx] << "'. Erro: " << e.what() << ". Ignorando." << std::endl;
                        }
                        catch (const std::out_of_range& e) {
                            std::cerr << "Aviso: Valor fora do intervalo para a metrica '" << metric << "' no benchmark '"
                                      << benchmark << "': '" << values[idx] << "'. Erro: " << e.what() << ". Ignorando." << std::endl;
                        }
                        // Não precisamos de um catch-all para '...', os catches específicos já cobrem as principais falhas de stod.
                    } else {
                        std::cerr << "Aviso: Indice de valor fora do limite para a metrica '" << metric << "' no benchmark '"
                                  << benchmark << "'. Ignorando." << std::endl;
                    }
                } else {
                    // std::cerr << "Aviso: Metrica '" << metric << "' nao encontrada no cabecalho para o benchmark '"
                    //           << benchmark << "'. Ignorando." << std::endl;
                }
            }
            if (!metrics.empty())
                benchmarks_data.emplace_back(benchmark, metrics);
            else {
                std::cerr << "Aviso: Nenhuma metrica valida encontrada para o benchmark '" << benchmark << "'. Nao adicionado." << std::endl;
            }
        } else {
            std::cerr << "Aviso: Nenhuma linha de dados apos o cabecalho em " << entry.path() << "/metrics.csv" << std::endl;
        }
    }

    drawing dw(*metrics_win);

    // Requisita redesenho quando seleciona outra métrica
    metric_selector.events().selected([&](const arg_combox&) {
        dw.update();
    });

    dw.draw([&](paint::graphics& graph) {
        graph.rectangle(true); // Limpa a área de desenho

        if (benchmarks_data.empty()) {
            graph.string({static_cast<int>(graph.width() / 2) - 100, static_cast<int>(graph.height() / 2)}, "Nenhum dado de benchmark encontrado.", colors::red);
            return;
        }

        // --- Variáveis de Layout do Gráfico ---
        int top_margin = 120;
        int origin_x = 80;
        int bottom_margin = 50;
        int origin_y = graph.height() - bottom_margin;

        int legend_width = 150;
        int right_margin_for_chart = 30;
        int chart_width_total = graph.width() - origin_x - legend_width - right_margin_for_chart;
        int chart_height = origin_y - top_margin;

        int chart_padding_x = 20;

        int effective_chart_width = chart_width_total - (2 * chart_padding_x);

        if (effective_chart_width <= 0 || chart_height <= 0) {
            graph.string({static_cast<int>(graph.width() / 2) - 100, static_cast<int>(graph.height() / 2)}, "Espaco insuficiente para o grafico.", colors::red);
            return;
        }

        std::string selected_metric = metric_selector.text(metric_selector.option());
        if (selected_metric.empty())
            selected_metric = "CPI_Médio";

        // --- Desenho do Título Dinâmico ---
        std::string title_text = "Métrica -- " + selected_metric;
        auto title_size = graph.text_extent_size(title_text);
        
        // Centraliza o título sobre a área efetiva das barras
        int title_x = origin_x + chart_padding_x + (effective_chart_width / 2) - (static_cast<int>(title_size.width) / 2);
        graph.string({title_x, 50}, title_text, colors::dark_blue);

        // Busca valor máximo da métrica para normalizar as barras
        double max_value = 0.0;
        for (const auto& [_, metrics] : benchmarks_data) {
            auto it = metrics.find(selected_metric);
            if (it != metrics.end() && it->second > max_value)
                max_value = it->second;
        }
        if (max_value <= 0.0)
            max_value = 1.0;

        // --- Ajuste dinâmico de Largura da Barra e Espaçamento ---
        int num_benchmarks = static_cast<int>(benchmarks_data.size());
        int min_bar_width = 10;
        int max_bar_width = 40;
        int min_spacing = 10;

        int bar_width;
        int spacing;

        if (num_benchmarks == 0) {
            bar_width = 0;
            spacing = 0;
        } else if (num_benchmarks == 1) {
            bar_width = std::min(max_bar_width, effective_chart_width / 2);
            spacing = 0;
        } else {
            bar_width = (effective_chart_width - (num_benchmarks - 1) * min_spacing) / num_benchmarks;
            bar_width = std::max(min_bar_width, std::min(max_bar_width, bar_width));

            spacing = (effective_chart_width - (num_benchmarks * bar_width)) / (num_benchmarks - 1);
            spacing = std::max(min_spacing, spacing);
        }

        if (num_benchmarks > 1 && spacing < min_spacing) {
            bar_width = (effective_chart_width - (num_benchmarks - 1) * min_spacing) / num_benchmarks;
            bar_width = std::max(min_bar_width, bar_width);
            spacing = min_spacing;
        }
        
        int total_bars_width = num_benchmarks * bar_width + (num_benchmarks > 0 ? (num_benchmarks - 1) * spacing : 0);
        int start_x = origin_x + chart_padding_x + std::max(0, (effective_chart_width - total_bars_width) / 2);

        // --- Desenho dos Eixos ---
        graph.line({origin_x, origin_y}, {origin_x + chart_width_total, origin_y}, colors::black);
        graph.line({origin_x, origin_y}, {origin_x, top_margin}, colors::black);

        // --- Linhas da Grade Horizontal e Rótulos do Eixo Y ---
        int num_horizontal_lines = 5;
        graph.string(point(origin_x - static_cast<int>(graph.text_extent_size("0.00").width) - 5, static_cast<int>(origin_y - graph.text_extent_size("0.00").height / 2)), "0.00", colors::black);

        for (int i = 1; i <= num_horizontal_lines; ++i) {
            int y = origin_y - (chart_height * i) / num_horizontal_lines;
            graph.line({origin_x, y}, {origin_x + chart_width_total, y}, colors::light_gray);

            std::string y_label = format_large_number(max_value * i / num_horizontal_lines);
            auto y_label_size = graph.text_extent_size(y_label);
            graph.string(point(origin_x - static_cast<int>(y_label_size.width) - 5, static_cast<int>(y - y_label_size.height / 2)), y_label, colors::black);
        }

        // Paleta de cores para barras
        std::vector<color> colors_palette = {
            colors::red, colors::green, colors::blue, colors::orange, colors::purple,
            colors::brown, colors::cyan, colors::magenta, colors::yellow, colors::dark_green,
            colors::lime_green, colors::deep_sky_blue, colors::hot_pink, colors::dark_goldenrod
        };

        // --- Desenha Barras com Valores ---
        int min_bar_height_for_visibility = 2;
        for (size_t i = 0; i < benchmarks_data.size(); ++i) {
            const auto& [benchmark, metrics] = benchmarks_data[i];
            auto it = metrics.find(selected_metric);
            if (it == metrics.end()) continue;

            double val = it->second;
            int bar_x = start_x + static_cast<int>(i) * (bar_width + spacing);
            int bar_h = static_cast<int>((val / max_value) * chart_height);

            if (val > 0 && bar_h < min_bar_height_for_visibility) {
                bar_h = min_bar_height_for_visibility;
            } else if (val == 0) {
                bar_h = 0;
            }
            
            int bar_y = origin_y - bar_h;

            color bar_color = colors_palette[i % colors_palette.size()];
            graph.rectangle(rectangle(bar_x, bar_y, bar_width, bar_h), true, bar_color);

            std::string value_str = format_large_number(val);
            auto val_size = graph.text_extent_size(value_str);
            int text_y = (bar_h < 30 && val > 0) ? (origin_y - 25) : (bar_y - 25);
            graph.string(point(bar_x + (bar_width - static_cast<int>(val_size.width)) / 2, text_y), value_str, colors::black);
        }

        // --- Legenda à direita ---
        int legend_x = graph.width() - legend_width;
        int legend_start_y = top_margin;
        int legend_box_size = 15;
        int legend_spacing = 25;

        for (size_t i = 0; i < benchmarks_data.size(); ++i) {
            const auto& [benchmark, _] = benchmarks_data[i];
            color bar_color = colors_palette[i % colors_palette.size()];
            int y = legend_start_y + static_cast<int>(i) * legend_spacing;
            if (y + legend_box_size > origin_y + (bottom_margin / 2)) break;

            graph.rectangle(rectangle(legend_x, y, legend_box_size, legend_box_size), true, bar_color);
            graph.string(point(legend_x + legend_box_size + 5, y), benchmark, colors::black);
        }
    });

    parent.enabled(false);
    metrics_win->events().unload([&] { parent.enabled(true); });
    metrics_win->show();
    API::modal_window(metrics_win->handle());
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
