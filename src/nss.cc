#include "nss.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>//

using std::ifstream;
using std::stringstream;

Nss::Nss()
{
    disk();

    string i = "";
    string d = "";
    string dt = "";
    string a = "";
    string p = "windows";
    string t = "";
    string pt = "";

    search(&i, &d, &dt, &a, &p, &t, &pt);

    for (size_t i = 0; i < test_ret.size(); i++)
        std::cout << test_ret[i] << std::endl;
}

Nss::~Nss()
{
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

bool Nss::search(const string *id, const string *description, const string *date,
                 const string *author, const string *platform, const string *type,
                 const string *port)
{
    bool is_not_found = false;
    string buf;

    for (size_t i = 0; i < ids.size(); i++) {
        if ((*id != "") && (ids[i].find(*id) == string::npos))
            is_not_found = true;
        if (!is_not_found && (*description != "")) {
            buf = descriptions[i]; 
            std::transform(buf.begin(), buf.end(), buf.begin(), ::tolower); 
            if (buf.find(*description) == string::npos)
                is_not_found = true;
        }
        if (!is_not_found && (*date != "") &&
            (dates[i].find(*date) == string::npos))
            is_not_found = true;
        if (!is_not_found && (*author != "") &&
            (authors[i].find(*author) == string::npos))
            is_not_found = true;
        if (!is_not_found && (*platform != "") &&
            (platforms[i].find(*platform) == string::npos))
            is_not_found = true;
        if (!is_not_found && (*type != "") &&
            (types[i].find(*type) == string::npos))
            is_not_found = true;
        if (!is_not_found && (*port != "") &&
            (ports[i].find(*port) == string::npos))
            is_not_found = true;
        
        if (!is_not_found)
            test_ret.push_back(ids[i] + "    " + dates[i] + "    " + platforms[i] + "    " + types[i] + "    " + descriptions[i]);
        else
            is_not_found = false;
    }

    return true;
}

bool Nss::cli(const string &cmd, const string &args)
{
    char buf[BUFSIZ];
    string command = cmd + " " + args + " 2>/dev/null";
   
    FILE *fp = popen(command.c_str(), "r");
    if (!fp)
        return false;
    
    /*
    while (!feof(fp))
        if (fgets(buf, BUFSIZ, fp) != NULL)
            ret += buf;
    */

    if (pclose(fp) != 0)
        return false;

    return true;
}
