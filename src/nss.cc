#include "nss.h"

#include <libxml++/libxml++.h>

#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <pwd.h>

#include <iostream>//

using std::ifstream;
using std::stringstream;

Nss::Nss() :
    is_ui_blocked(false)
{
    ui.main();

    if (!config()) {
        ui.status("CONFIG FILE ERROR | <CE> EXIT");
        curs_set(0);
        is_ui_blocked = true;
    } else {
        if (!csv()) {
            ui.status("CSV FILE ERROR | <CE> EXIT");
            curs_set(0);
            is_ui_blocked = true;
        }
    }
    driver();
}

Nss::~Nss()
{
}

bool Nss::config()
{
    string line;
    vector<string> configs = {"git_path=", "cp_path=", "editor="};
    ifstream file;
    
    passwd *pw = getpwuid(getuid());
    string home_path = pw->pw_dir;
    
    file.open(home_path + "/.nss");
    if (file.is_open()) {
        for (size_t i = 0; i < configs.size(); i++) {
            while (getline(file, line)) {
                if (line.find(configs[i]) != string::npos) {
                    if ((line.erase(0, configs[i].size())).size() != 0) {
                        user_configs.push_back(line);
                        break;
                    } else {
                        file.close();
                        return false;
                    }
                } else {
                    file.close();
                    return false;
                }
            }
        }
        file.close();
    } else {
        return false;
    }

    return true;
} 

bool Nss::csv()
{
    int n;
    size_t p;
    bool is_first_line = true;
    string line;
    string token;
    string delimiter = ",";
    stringstream ss;
    ifstream file;
    
    vector<string>().swap(ids);
    vector<string>().swap(files);
    vector<string>().swap(descriptions);
    vector<string>().swap(dates);
    vector<string>().swap(authors);
    vector<string>().swap(platforms);
    vector<string>().swap(types);
    vector<string>().swap(ports);
    
    file.open(user_configs[0] + "files.csv");
    if (file.is_open()) {
        ss << file.rdbuf();
        while (std::getline(ss, line, '\n')) {
            if (!is_first_line) {
                n = 0;
                while ((p = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, p);
                    switch(n)
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
                    ++n;
                }
                ports.push_back(line);
            } else {
                is_first_line = false;
            }
        }
        file.close();
    } else {
        return false;
    }

    return true;
}

void Nss::driver()
{ 
    int key;
    int c_field = 0;
    vector<string> terms;
    
    do {
        prefresh(ui.window, 0, 0, 0, 0, (LINES - 1), (COLS - 1));
        key = wgetch(ui.window);
        if (!is_ui_blocked) {
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
                            mvwdelch(ui.window, 0, 0);
                        curs_set(1);
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
                case KEY_BACKSPACE:
                case KEY_DELCHAR:
                case KEY_DC:
                    form_driver(ui.form, REQ_LEFT_CHAR);
                    form_driver(ui.form, REQ_DEL_CHAR);
                    break;
                case KEY_RETURN:
                    if (c_field == 8) {
                        bool has_terms = false;
                        bool is_xml = false;
                        vector<string>().swap(terms);
                        for (int i = 0; i <= 7; i++) { 
                            terms.push_back(clear_whitespaces(field_buffer(ui.fields[i], 0)));
                            if (terms[i] != "")
                                has_terms = true;
                        }
                        if (!has_terms)
                            break;
                        else if (terms[7] != "")
                            is_xml = true;
                        if (search(&terms, is_xml))
                            ui.result(&ids, &files, &descriptions,
                                      &dates, &platforms, &types,
                                      &results, &user_configs);
                        else
                            ui.status("NO RESULT");
                    }
                    break;
                case KEY_UPDATE:
                    if (update()) {
                        if (csv()) {
                            ui.status("DB UPDATED");
                        } else {    
                            ui.status("CSV FILE ERROR | <CE> EXIT");
                            is_ui_blocked = true;
                        }
                    } else {
                        ui.status("UPDATE DB ERROR");
                    }
                    break;
                default:
                    form_driver(ui.form, key);
                    break;
            }
        }
    } while (key != KEY_QUIT);
}

bool Nss::search(vector<string> *terms, bool is_xml)
{
    bool is_not_found = false;
    bool is_invalid_term = false;
    string first_term;
    string second_term;
    string term;
    string buf;
    stringstream ss;
    vector<int> n_results;
    const vector<string> invalid_terms =
    {
        "microsoft",
        "windows",
        "linux",
        "oracle",
        "apache",
        "http",
        "ftp",
        "ssh",
        "net",
        "wireless",
        "wifi"
    };

    vector<int>().swap(results);
    
    if (!is_xml) {
        for (size_t i = 0; i < ids.size(); i++) {
            if (((*terms)[0] != "") && (ids[i].find((*terms)[0]) == string::npos))
                is_not_found = true;
            if (((*terms)[1] != "") && !is_not_found) {
                buf = descriptions[i]; 
                std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower); 
                if (buf.find((*terms)[1]) == string::npos)
                    is_not_found = true;
            }
            if (((*terms)[2] != "") && !is_not_found &&
                (dates[i].find((*terms)[2]) == string::npos))
                is_not_found = true;
            if (((*terms)[3] != "") && !is_not_found &&
                (authors[i].find((*terms)[3]) == string::npos))
                is_not_found = true;
            if (((*terms)[4] != "") && !is_not_found &&
                (platforms[i].find((*terms)[4]) == string::npos))
                is_not_found = true;
            if (((*terms)[5] != "") && !is_not_found &&
                (types[i].find((*terms)[5]) == string::npos))
                is_not_found = true;
            if (((*terms)[6] != "") && !is_not_found &&
                (ports[i].find((*terms)[6]) == string::npos))
                is_not_found = true;
            
            if (!is_not_found)
                results.push_back(i);
            else
                is_not_found = false;
        }
    } else {
        buf = (*terms)[7];
        vector<string>().swap((*terms));
        if (xml(buf, &terms)) {
            for (size_t i = 0; i < terms->size(); i++) {
                ss.str((*terms)[i]);
                ss >> first_term >> second_term;
                term = ss.str();
                for (size_t ii = 0; ii < 4; ii++) {
                    vector<int>().swap(n_results);
                    switch(ii)
                    {
                        case 1:
                            term = first_term + string(" ") + second_term;
                            break;
                        case 2:
                            term = first_term;
                            break;
                        case 3:
                            term = second_term;
                            break;
                    }
                    for (size_t iii = 0; iii < invalid_terms.size(); iii++)
                        if (term == invalid_terms[iii])
                            is_invalid_term = true;
                    if (!is_invalid_term) {
                        for (size_t iiii = 0; iiii < ids.size(); iiii++) {
                            buf = descriptions[iiii]; 
                            std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower); 
                            if (buf.find(term) != string::npos)
                                n_results.push_back(iiii);
                        }
                        if (n_results.size() < 100)
                            for (size_t iiiii = 0; iiiii < n_results.size(); iiiii++)
                                results.push_back(n_results[iiiii]);
                    }
                } // TODO: FALTA LEER EL ULTIMO TERM (Dropbear) Y LIMPIAR (/-_) EN xml().
            }
        } else {
            return false;
        }
    }

    if (results.size() == 0)
        return false;

    return true;
}

bool Nss::xml(const string &path, vector<string> **terms)
{
    string buf;
    const vector<string> xpaths =
    {
        "/nmaprun/host/ports/port/state",
        "/nmaprun/host/ports/port/service"
    };
    const vector<string> xattributes = {"state", "product", "version"};

    try {
        xmlpp::DomParser parser;
        parser.parse_file(path);
        xmlpp::Node *root = parser.get_document()->get_root_node();
        
        xmlpp::Node::NodeSet node;
        xmlpp::Element *element;
        xmlpp::Attribute *attribute;

        node = root->find(xpaths[0]);

        if (node.size() == 0) {
            return false;
        } else {
            for (size_t i = 0; i < node.size(); i++) {
                node = root->find(xpaths[0]);
                element = (xmlpp::Element *)node.at(i);
                attribute = element->get_attribute(xattributes[0]);
                if (attribute) {
                    if (attribute->get_value() == "open") {
                        node = root->find(xpaths[1]);
                        element = (xmlpp::Element *)node.at(i);
                        for (size_t ii = 1; ii < 3; ii++) {
                            attribute = element->get_attribute(xattributes[ii]);
                            if (attribute)
                                buf = buf + string(" ") + attribute->get_value();
                        }
                        if (buf != "") {
                            std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower); 
                            (*terms)->push_back(buf);
                        }
                        buf.clear();
                    }
                }
            }
        }
    } catch (...) {
        return false;
    }

    return true;
}

bool Nss::update()
{
    string git_remote = "https://github.com/offensive-security/exploit-database.git";
    string cmd = "(cd " + user_configs[0] + string(" && ") +
                 string("if [[ ''$( git remote -v )'' != *''") + git_remote + string("''* ]]; then ") +
                 string("git init && git remote add origin ") + git_remote + string("; fi && ") + 
                 string("git checkout -- . && git pull origin master)") +
                 string(" > /dev/null 2>&1"); 
                
    ui.status("UPDATING DB");
    curs_set(0);
    
    FILE *fp = popen(cmd.c_str(), "w");
    if (!fp) 
        return false;
    if (pclose(fp) != 0)
        return false;

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
