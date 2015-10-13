#ifndef MISCLIB_H
#define MISCLIB_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

class MiscLib
{
    public:
        MiscLib();
        static std::vector<std::string> split(const std::string&, const char);

        static inline std::string &ltrim(std::string s) {
            s.erase(s.begin(), std::find_if(
                    s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
            return s;
        }

        template<class T, class T2>
        static void printMap(std::map<T,T2> myMap){
            for(auto elem : myMap){
                std::cout << elem.first << " " << elem.second << "\n";
            }
        }

    protected:
    private:
};

#endif // MISCLIB_H
