#include "ui.h"

#include <sstream>
#include <iomanip>

using std::stringstream;

Ui::Ui() :
    has_status(false),
    field_width(43),
    button_width(21),
    n_results(0)
{
    int rows;
    int cols;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    
    if (getenv("ESCDELAY") == NULL)
        set_escdelay(25);
    
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_BLACK, COLOR_WHITE);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_WHITE);
    }
    
    if (LINES < 49)
        rows = 49;
    else
        rows = LINES;
    
    if (COLS < 175)
        cols = 175;
    else
        cols = COLS;

    window = newpad(rows, cols);
    keypad(window, TRUE);
    
    refresh();
}

Ui::~Ui()
{
    clear_form();
    delwin(window);
    endwin();
}

void Ui::main()
{
    int rows;
    int cols;
    
    start_x = (COLS / 2.63);
    start_y = (LINES / 2.45);
    
    fields[0] = new_field(1, field_width,  start_y,        start_x, 0, 0);
    fields[1] = new_field(1, field_width,  (start_y + 2),  start_x, 0, 0);
    fields[2] = new_field(1, field_width,  (start_y + 4),  start_x, 0, 0);
    fields[3] = new_field(1, field_width,  (start_y + 6),  start_x, 0, 0);
    fields[4] = new_field(1, field_width,  (start_y + 8),  start_x, 0, 0);
    fields[5] = new_field(1, field_width,  (start_y + 10), start_x, 0, 0);
    fields[6] = new_field(1, field_width,  (start_y + 12), start_x, 0, 0);
    fields[7] = new_field(1, field_width,  (start_y + 14), start_x, 0, 0);
    fields[8] = new_field(1, button_width, (start_y + 16), (start_x + 11), 0, 0);
    fields[9] = NULL;
    
    set_field_just(fields[0], JUSTIFY_CENTER);
    set_field_just(fields[1], JUSTIFY_CENTER);
    set_field_just(fields[2], JUSTIFY_CENTER);
    set_field_just(fields[3], JUSTIFY_CENTER);
    set_field_just(fields[4], JUSTIFY_CENTER);
    set_field_just(fields[5], JUSTIFY_CENTER);
    set_field_just(fields[6], JUSTIFY_CENTER);
    set_field_just(fields[7], JUSTIFY_CENTER);
    field_opts_off(fields[8], O_EDIT);
    
    for (int i = 0; i < 8; i++)
        field_opts_off(fields[i], O_AUTOSKIP);
    
    set_field_back(fields[0], COLOR_PAIR(2));
    set_field_back(fields[1], COLOR_PAIR(1));
    set_field_back(fields[2], COLOR_PAIR(1));
    set_field_back(fields[3], COLOR_PAIR(1));
    set_field_back(fields[4], COLOR_PAIR(1));
    set_field_back(fields[5], COLOR_PAIR(1));
    set_field_back(fields[6], COLOR_PAIR(1));
    set_field_back(fields[7], COLOR_PAIR(1));
    set_field_back(fields[8], COLOR_PAIR(1));
    
    stringstream ss;
    ss << std::setw(button_width / 2.7) << std::setfill(' ') << "" << "SEARCH";
    set_field_buffer(fields[8], 0, ss.str().c_str());
    
    form = new_form(fields);
    scale_form(form, &rows, &cols);
    set_form_win(form, window);
    set_form_sub(form, derwin(window, rows, cols, 0, 0));
    post_form(form);
    
    mvwprintw(window, start_y,        (start_x - 4),  "ID");
    mvwprintw(window, (start_y + 2),  (start_x - 7),  "TITLE");
    mvwprintw(window, (start_y + 4),  (start_x - 6),  "DATE");
    mvwprintw(window, (start_y + 6),  (start_x - 8),  "AUTHOR");
    mvwprintw(window, (start_y + 8),  (start_x - 10), "PLATFORM");
    mvwprintw(window, (start_y + 10), (start_x - 6),  "TYPE");
    mvwprintw(window, (start_y + 12), (start_x - 6),  "PORT");
    mvwprintw(window, (start_y + 14), (start_x - 5),  "XML");

    prefresh(window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
    
    status("<CE> EXIT", 0);
    
    set_current_field(form, fields[0]);
    form_driver(form, REQ_END_LINE);
}


void Ui::result(const vector<string> *ids, const vector<string> *files,
                const vector<string> *descriptions, const vector<string> *dates,
                const vector<string> *authors, const vector<string> *platforms,
                const vector<string> *types, const vector<int> *results)
{
    int key;
    unsigned int c_item = 0;

    n_results = results->size();
    
    start_y = (LINES / 10);

    items = (WINDOW **) malloc ((n_results + 1) * sizeof(WINDOW *));
    //items[0] = newpad((n_results + start_y), COLS);
    items[0] = newpad((start_y + n_results + LINES), COLS);
    keypad(items[0], true);
    
    for (size_t i = 1; i < n_results; i++) {
        items[i] = subpad(items[0], 1, (COLS - 5), (i - 1), 0);
        mvwprintw(items[i], 0, 0, "%s", (*descriptions)[(*results)[i - 1]].c_str());
        mvwprintw(items[i], 0, (COLS - 44), "%s", (*dates)[(*results)[i - 1]].c_str());
        mvwprintw(items[i], 0, (COLS - 30), "%s", (*platforms)[(*results)[i - 1]].c_str());
        mvwprintw(items[i], 0, (COLS - 18), "%s", (*types)[(*results)[i - 1]].c_str());
    }
    
    wbkgd(items[1], COLOR_PAIR(2));
    wbkgd(items[1], A_REVERSE);
    
    prefresh(items[0], 0, 0, start_y, 5, (LINES - start_y), (COLS - 5));
    
    status("<INTRO> OPEN | <CC> COPY | <ESC> BACK", 0);
    
    do {
        prefresh(items[0], c_item, 0, start_y, 5, (LINES - start_y), (COLS - 5));
        key = wgetch(items[0]);
        switch(key)
        {
            case KEY_LEFT:
                key = KEY_ESCAPE;
                break;
            case KEY_UP:
                if (c_item > 0) {
                    wbkgd(items[c_item + 1], COLOR_PAIR(2));
                    --c_item;
                }
                break;
            case KEY_DOWN:
                if (c_item < (n_results - 2)) {
                    wbkgd(items[c_item + 2], A_REVERSE);
                    ++c_item;
                }
                break;
            case KEY_RIGHT:
            case KEY_RETURN:
                system("bash -t -c 'vim /home/user/file'");
                //report_details(&ids, (c_item + 1), 0); //n
                break;
            default:
                break;
        }
    } while (key != KEY_ESCAPE);
   
    clear_items();
}

bool Ui_result_do(const vector<string> **files, unsigned int c_item)
{
    string test = (**files)[c_item];
    return true;
}

void Ui::marker(bool show)
{
    int row;
    int col;
    int cols;
    
    field_info(current_field(form), NULL, &cols, &row, &col, NULL, NULL);

    if (show) {
        mvwaddch(window, row, col, ACS_CKBOARD);
        mvwaddch(window, row, (col + (cols - 1)), ACS_CKBOARD);
        curs_set(0);
    } else {
        wattron(window, COLOR_PAIR(1));
        mvwprintw(window, row, col, " ");
        mvwprintw(window, row, (col + (cols - 1)), " ");
        wattroff(window, COLOR_PAIR(1));
        curs_set(1);
    }
}

void Ui::status(const string &s, int n_tabs)
{
    if (has_status) {
        for (int i = 0; i < COLS; i++)
            mvwdelch(window, (0 + n_tabs), 0);
        prefresh(window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
    }

    int s_x = (COLS - s.size()) / 2; 
    
    mvwprintw(window, (0 + n_tabs), s_x, s.c_str());
    
    prefresh(window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
    
    has_status = true;
}

void Ui::clear_items()
{
    for (size_t i = 1; i < (n_results + 1); i++)
        delwin(items[i]);
    delwin(items[0]);
    free(items);
}

void Ui::clear_form()
{
    size_t n = field_count(form);

    unpost_form(form);
    free_form(form);
    for (size_t i = 0; i < n; i++)
        free_field(fields[i]);
}
