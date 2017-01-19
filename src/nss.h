#ifndef NSS_H
#define NSS_H

#include "ui.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class Nss
{
public:
    Nss();
    ~Nss();

private:
    Ui ui;
   
    bool is_ui_blocked;

    vector<int> results;
    
    vector<string> user_configs;
    vector<string> ids;
    vector<string> files;
    vector<string> descriptions;
    vector<string> dates;
    vector<string> authors;
    vector<string> platforms;
    vector<string> types;
    vector<string> ports;

    bool config();
    bool csv();
    void driver();
    bool search();
    bool update();
    string clear_whitespaces(const char *c);
};

#endif
