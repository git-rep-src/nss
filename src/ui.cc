#include "ui.h"

#include <sstream>
#include <iomanip>

using std::stringstream;

Ui::Ui() :
    has_status(false),
    n_results(0)
{
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
    
    if (LINES < 24)
        w_rows = 24;
    else
        w_rows = LINES;
    
    if (COLS < 67)
        w_cols = 67;
    else
        w_cols = COLS;

    window = newpad(w_rows, w_cols);
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
    // TODO: RESIZE.

    int rows;
    int cols;
    int start_x = ((w_cols - 43) / 2);
    int start_y = ((w_rows - 16) / 2);
    
    fields[0] = new_field(1, 43, start_y,        start_x, 0, 0);
    fields[1] = new_field(1, 43, (start_y + 2),  start_x, 0, 0);
    fields[2] = new_field(1, 43, (start_y + 4),  start_x, 0, 0);
    fields[3] = new_field(1, 43, (start_y + 6),  start_x, 0, 0);
    fields[4] = new_field(1, 43, (start_y + 8),  start_x, 0, 0);
    fields[5] = new_field(1, 43, (start_y + 10), start_x, 0, 0);
    fields[6] = new_field(1, 43, (start_y + 12), start_x, 0, 0);
    fields[7] = new_field(1, 43, (start_y + 14), start_x, 0, 0);
    fields[8] = new_field(1, 21, (start_y + 16), (start_x + 11), 0, 0);
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
    ss << std::setw(7.77) << std::setfill(' ') << "" << "SEARCH";
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
    mvwprintw(window, (start_y + 14), (start_x - 6),  "NMAP");

    prefresh(window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
    
    status("<CU> UPDATE DB | <CE> EXIT");

    set_current_field(form, fields[0]);
    form_driver(form, REQ_END_LINE);
}


void Ui::result(const vector<string> *ids, const vector<string> *files,
                vector<string> *descriptions, const vector<string> *dates,
                const vector<string> *platforms, const vector<string> *types,
                const vector<int> *results, const vector<string> *user_configs)
{
    bool has_copy;
    bool was_copy;
    int key;
    int start_y = (LINES / 10);
    size_t p;
    unsigned int c_item = 0;
    string cmd;

    n_results = results->size();

    items = (WINDOW **) malloc ((n_results + 1) * sizeof(WINDOW *));
    items[0] = newpad((start_y + n_results + LINES), COLS);
    keypad(items[0], true);

    for (size_t i = 1; i <= n_results; i++) {
        items[i] = subpad(items[0], 1, (COLS - 5), (i - 1), 0);
        p = (COLS - 54);
        if ((*descriptions)[(*results)[i - 1]].size() >= p)
            (*descriptions)[(*results)[i - 1]].replace((p - 3),
                                                       ((*descriptions)[(*results)[i - 1]].size() -
                                                        (p - 3)), "...");
        mvwprintw(items[i], 0, 0, "%s", (*descriptions)[(*results)[i - 1]].c_str());
        mvwprintw(items[i], 0, (COLS - 50), "%s", (*dates)[(*results)[i - 1]].c_str());
        mvwprintw(items[i], 0, (COLS - 36), "%s", (*platforms)[(*results)[i - 1]].c_str());
        mvwprintw(items[i], 0, (COLS - 24), "%s", (*types)[(*results)[i - 1]].c_str());
        p = (*files)[(*results)[i - 1]].find(".");
        mvwprintw(items[i], 0, (COLS - 13), "%s",
                  (*files)[(*results)[i - 1]].substr((p + 1),
                  (*files)[(*results)[i - 1]].size()).c_str());
    }
    
    wbkgd(items[1], COLOR_PAIR(2));
    wbkgd(items[1], A_REVERSE);
    
    prefresh(items[0], 0, 0, start_y, 5, (LINES - start_y), (COLS - 5));
    
    status("<INTRO> OPEN | <CP> COPY | <ESC> BACK");
    has_status = false;
    
    do {
        prefresh(items[0], c_item, 0, start_y, 5, (LINES - start_y), (COLS - 5));
        key = wgetch(items[0]);
        switch(key)
        {
            case KEY_LEFT:
                key = KEY_ESCAPE;
                break;
            case KEY_UP:
            case KEY_DOWN:
                if (has_status) {
                    for (int i = 0; i < COLS; i++)
                        mvwdelch(window, 0, 0);
                    status("<INTRO> OPEN | <CP> COPY | <ESC> BACK");
                    has_status = false;
                }
                if (key == KEY_UP) { 
                    if (c_item > 0) {
                        wbkgd(items[c_item + 1], COLOR_PAIR(2));
                        --c_item;
                    }
                } else {
                    if (c_item < (n_results - 1)) {
                        wbkgd(items[c_item + 2], A_REVERSE);
                        ++c_item;
                    }
                }
                break;
            case KEY_RIGHT:
            case KEY_RETURN:
                was_copy = false;
                for (size_t i = 0; i < copieds.size(); i++)
                    if ((*ids)[(*results)[c_item]] == copieds[i])
                        was_copy = true;
                if (was_copy) { 
                    cmd = (*user_configs)[2] +
                          (*user_configs)[1] +
                          (*ids)[(*results)[c_item]] +
                          string(".*");
                } else {
                    cmd = (*user_configs)[2] +
                          (*user_configs)[0] +
                          (*files)[(*results)[c_item]];
                }
                if (!cli(cmd, true)) {
                    has_status = true;
                    status("OPEN FILE ERROR");
                }
                break;
            case KEY_CP:
                cmd = "cp -f " +
                      (*user_configs)[0] +
                      (*files)[(*results)[c_item]] +
                      string(" ") +
                      (*user_configs)[1] +
                      string(" 2>/dev/null");
                if (cli(cmd, false)) {
                    has_copy = false;
                    for (size_t i = 0; i < copieds.size(); i++)
                        if ((*ids)[(*results)[c_item]] == copieds[i])
                            has_copy = true;
                    if (!has_copy)
                        copieds.push_back((*ids)[(*results)[c_item]]);
                    has_status = true;
                    status("FILE COPYED");
                } else {
                    has_status = true;
                    status("COPY FILE ERROR");
                }
                break;
            default:
                break;
        }
    } while (key != KEY_ESCAPE);
   
    clear_items();
    
    for (int i = 0; i < COLS; i++)
        mvwdelch(window, 0, 0);

    has_status = false;
}

void Ui::status(const string &s)
{
    if (has_status) {
        for (int i = 0; i < COLS; i++)
            mvwdelch(window, 0, 0);
        prefresh(window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
    }

    int s_x = (COLS - s.size()) / 2; 
    
    mvwprintw(window, 0, s_x, s.c_str());
    
    prefresh(window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
    
    has_status = true;
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

bool Ui::cli(const string &cmd, bool is_editor)
{
    FILE *fp = popen(cmd.c_str(), "w");
    if (!fp) 
        return false;
    if (pclose(fp) != 0)
        return false;
    
    if (is_editor)
        endwin();
   
    return true;
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
