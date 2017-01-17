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
    
    vector<int> results;
    
    vector<string> ids;
    vector<string> files;
    vector<string> descriptions;
    vector<string> dates;
    vector<string> authors;
    vector<string> platforms;
    vector<string> types;
    vector<string> ports;
    
    void driver();
    void disk();
    bool search();
    bool cli(const string &cmd, const string &args);
    string clear_whitespaces(const char *c);
};

#endif
