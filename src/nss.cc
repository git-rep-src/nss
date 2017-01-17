#include "nss.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>//

using std::ifstream;
using std::stringstream;

Nss::Nss()
{
    ui.main();
    disk();
    driver();
}

Nss::~Nss()
{
}

void Nss::driver()
{ 
    int key;
    int c_field = 0;
    int n_tabs = 0;

    do {
        prefresh(ui.window, n_tabs, 0, 0, 0, (LINES - 1), (COLS - 1));
        key = wgetch(ui.window);
        switch(key)
        {
            case KEY_LEFT:
                form_driver(ui.form, REQ_PREV_CHAR);
                break;
            case KEY_RIGHT:
                form_driver(ui.form, REQ_NEXT_CHAR);
                break;
            case KEY_UP:
            case KEY_DOWN:
                if (ui.has_status) {
                    for (int i = 0; i < COLS; i++)
                        mvwdelch(ui.window, (0 + n_tabs), 0);
                    ui.has_status = false;
                }
                if (c_field == 8)
                    ui.marker(false);
                else
                    set_field_back(ui.fields[c_field], COLOR_PAIR(1));
                if (key == KEY_UP) {
                    if (c_field == 0)
                        c_field = (field_count(ui.form) - 1);
                    else
                        --c_field;
                    form_driver(ui.form, REQ_PREV_FIELD);
                    form_driver(ui.form, REQ_END_LINE);
                } else {
                    if (c_field == (field_count(ui.form) - 1))
                        c_field = 0;
                    else
                        ++c_field;
                    form_driver(ui.form, REQ_NEXT_FIELD);
                    form_driver(ui.form, REQ_END_LINE);
                }
                if (c_field == 8)
                    ui.marker();
                else
                    set_field_back(ui.fields[c_field], COLOR_PAIR(2));
                break;
            case KEY_UP_SCREEN:
                if (ui.has_status) {
                    for (int i = 0; i < COLS; i++)
                        mvwdelch(ui.window, (0 + n_tabs), 0);
                    ui.has_status = false;
                }
                if (n_tabs > 0)
                    --n_tabs;
                break;
            case KEY_DOWN_SCREEN:
                if (ui.has_status) {
                    for (int i = 0; i < COLS; i++)
                        mvwdelch(ui.window, (0 + n_tabs), 0);
                    ui.has_status = false;
                }
                if ((LINES + n_tabs) < 15)
                    ++n_tabs;
                prefresh(ui.window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
                break;
            case KEY_BACKSPACE:
            case KEY_DELCHAR:
            case KEY_DC:
                form_driver(ui.form, REQ_LEFT_CHAR);
                form_driver(ui.form, REQ_DEL_CHAR);
                break;
            case KEY_RETURN:
                if (search())
                    ui.result(&ids, &files, &descriptions, &dates, &authors, &platforms, &types, &results);
                else
                    ui.status("NO RESULT", n_tabs);
                break;
            default:
                form_driver(ui.form, key);
                break;
        }
    } while (key != KEY_QUIT);
}

void Nss::disk()
{
    int i;
    size_t p;
    bool is_first_line = true;

    ifstream file;
    stringstream ss;
    string line;
    string token;
    string delimiter = ",";

    file.open("/home/user/nss/.etc/files.csv");
    
    if (file.is_open()) {
        ss << file.rdbuf();
        while (std::getline(ss, line, '\n')) {
            if (!is_first_line) {
                i = 0;
                while ((p = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, p);
                    switch(i)
                    {
                        case 0:
                            ids.push_back(token);
                            break;
                        case 1:
                            files.push_back(token);
                            break;
                        case 2:
                            token.erase(remove(token.begin(), token.end(), '\"' ), token.end());
                            descriptions.push_back(token);
                            break;
                        case 3:
                            dates.push_back(token);
                            break;
                        case 4:
                            authors.push_back(token);
                            break;
                        case 5:
                            platforms.push_back(token);
                            break;
                        case 6:
                            types.push_back(token);
                            break;
                        default:
                            break;
                    }
                    line.erase(0, (p + delimiter.length()));
                    ++i;
                }
                ports.push_back(line);
            } else {
                is_first_line = false;
            }
        }
        file.close();
    }
}

bool Nss::search()
{
    bool is_not_found = false;
    string buf;
    vector<string> terms;

    // TODO: XML FIELD
    for (int i = 0; i <= 6; i++) 
        terms.push_back(clear_whitespaces(field_buffer(ui.fields[i], 0)));
    
    vector<int>().swap(results);
    
    for (size_t i = 0; i < ids.size(); i++) {
        if ((terms[0] != "") && (ids[i].find(terms[0]) == string::npos))
            is_not_found = true;
        if ((terms[1] != "") && !is_not_found) {
            buf = descriptions[i]; 
            std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower); 
            if (buf.find(terms[1]) == string::npos)
                is_not_found = true;
        }
        if ((terms[2] != "") && !is_not_found &&
            (dates[i].find(terms[2]) == string::npos))
            is_not_found = true;
        if ((terms[3] != "") && !is_not_found &&
            (authors[i].find(terms[3]) == string::npos))
            is_not_found = true;
        if ((terms[4] != "") && !is_not_found &&
            (platforms[i].find(terms[4]) == string::npos))
            is_not_found = true;
        if ((terms[5] != "") && !is_not_found &&
            (types[i].find(terms[5]) == string::npos))
            is_not_found = true;
        if ((terms[6] != "") && !is_not_found &&
            (ports[i].find(terms[6]) == string::npos))
            is_not_found = true;
        
        if (!is_not_found)
            results.push_back(i);
        else
            is_not_found = false;
    }

    if (results.size() == 0)
        return false;

    return true;
}

bool Nss::cli(const string &cmd, const string &args)
{
    //char buf[BUFSIZ];
    //string command = cmd + " " + args + " 2>/dev/null";
   
    //FILE *fp = popen(command.c_str(), "r");
    //if (!fp)
        //return false;
    
    /*
    while (!feof(fp))
        if (fgets(buf, BUFSIZ, fp) != NULL)
            ret += buf;
    */

    //if (pclose(fp) != 0)
        //return false;

    system("vim /home/user/file");
    return true;
}

inline string Nss::clear_whitespaces(const char *c)
{
    string str(c);
    
    str.erase(unique(str.begin(), str.end(), [] (char a, char b) {
                     return isspace(a) && isspace(b);}), str.end());
    str.replace((str.size() - 1), 1, "");

    return str;
}
