#ifndef UI_H
#define UI_H

#include <form.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

const int KEY_UPDATE = 21;
const int KEY_CP = 16;
const int KEY_DELCHAR = 127;
const int KEY_RETURN = 10;
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
                const vector<string> *type, const vector<int> *results,
                const vector<string> *user_configs);
    void marker(bool show = true);
    void status(const string &s);
    void clear_items();
    void clear_form();

private:
    int start_x;
    int start_y;
    int field_width;
    int button_width;

    size_t n_results;

    WINDOW **items = NULL;

    bool cli(const string &cmd, bool is_editor);
};

#endif
