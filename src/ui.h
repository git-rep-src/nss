#ifndef UI_H
#define UI_H

#include <form.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

const int KEY_DELCHAR = 127;
const int KEY_RETURN = 10;
const int KEY_UP_SCREEN = 21;
const int KEY_DOWN_SCREEN = 4;
const int KEY_ESCAPE = 27;
const int KEY_QUIT = 5;

class Ui
{
public:
    Ui();
    ~Ui();
   
    bool has_status;
    
    WINDOW *window;
    FORM *form;
    FIELD *fields[10];
    
    void main();
    void result(const vector<string> *ids, const vector<string> *files,
                const vector<string> *descriptions, const vector<string> *dates,
                const vector<string> *authors, const vector<string> *platforms,
                const vector<string> *type, const vector<int> *results);
    void marker(bool show = true);
    void status(const string &s, int n_tabs);
    void clear_items();
    void clear_form();

private:
    int start_x;
    int start_y;
    int field_width;
    int button_width;

    size_t n_results;

    WINDOW **items = NULL;

    bool result_do(const vector<string> **files, unsigned int c_item);
};

#endif
