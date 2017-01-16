#ifndef NSS_H
#define NSS_H

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
    vector<string> ids;
    vector<string> files;
    vector<string> descriptions;
    vector<string> dates;
    vector<string> authors;
    vector<string> platforms;
    vector<string> types;
    vector<string> ports;

    vector<string> test_ret;

    void disk();
    bool search(const string *id, const string *description, const string *date,
                const string *author, const string *platform, const string *type,
                const string *port);
    bool cli(const string &cmd, const string &args);
};

#endif
