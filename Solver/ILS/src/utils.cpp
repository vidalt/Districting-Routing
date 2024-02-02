#include "utils.h"
#include <iostream>

vector<string> utils::splitString(const string &s, const char &c)
{
    string buff{""};
    vector<string> v;
    for (auto n : s)
    {
        if (n != c)
            buff += n;
        else if (n == c && buff != "")
        {
            v.push_back(buff);
            buff = "";
        }
    }
    if (buff != "")
        v.push_back(buff);
    return v;
}

ifstream utils::openCommonFile(string path)
{
    ifstream instanceFile = ifstream();
    instanceFile.open(path);
    
    for (int iter=0; iter<100 && !instanceFile.is_open(); iter++)
    {
        instanceFile.open(path);
    }

    return instanceFile;
}
