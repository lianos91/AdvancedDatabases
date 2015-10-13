
#include "MiscLib.h"
#include <string>
#include <sstream>
#include <iostream>       // std::cout
#include <vector>
using namespace std;

vector<string> split2(const string &s, const char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
vector<string> MiscLib::split(const string &s, const char delim) {
    vector<string> elems;
    split2(s, delim, elems);
    return elems;
}


