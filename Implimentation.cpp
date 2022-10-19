#include <iostream>
#include <fstream>
#include <map>
#include <stack>
#include <string>
using namespace std;

std::ofstream file_o;
std::map<std::string, std::string> mp;

void file_end()
{
    file_o.close();
    std::ifstream f;
    f.open("file_end");
    std::string str;
    while (!f.eof()) {
        std::getline(f, str);
        std::cout << str << '\n';
    }
    f.close();
}

std::string HELPERS(std::string str, int& i)
{
    std::string tmp;
    while (i != str.size()) {
            if (str[i] == '(') {
                ++i;
            } else if (str[i] == ')') {
                ++i;
                break;
            }
            tmp += str[i++];
        }
    return tmp;
}

void define_change(std::string str, std::string first1, std::string second1)
{
    int in = 0;
    if (second1 == "" || second1[0] != '(') {
        if (second1 == "") {
            in = str.find(first1);
            str.erase(in, first1.size());
            str.insert(in, second1);
        } else {
            in = str.find(first1);
            str.erase(in, first1.size());
            str.insert(in, second1);
        }
    } else {
        in = str.find(first1);
        str.erase(in, first1.size());
        int i = in;
        std::string tmp = HELPERS(str, i);
        str.erase(in, i - in);
        int j = 0;
        std::string tmp_second = second1;
        std::string str_second = HELPERS(second1, j);
        tmp_second.erase(0, j);
        int k = 0;
        int cat = 0;
        while (k != tmp_second.size()) {
            if ((cat = tmp_second.find(str_second)) != -1) {
                tmp_second.erase(cat, str_second.size());
                tmp_second.insert(cat, tmp);
            } else {
                break;
            }
            ++k;
        }
        str.insert(in, tmp_second);
    }
    if (str.find(first1) != -1) {
        define_change(str, first1, second1);
    } else {
        file_o << str << '\n';
    }
}

void find_define(std::string str)
{
    int index = 0;
    std::string tmp;
    std::string tmp1 = "";
    char symbol = '(';
        str.erase(index, 8);
        if (str.find(symbol) != -1) {
            index = 0;
            while (str[index] != '(') {
                tmp += str[index++];
            }
            str.erase(0, index);
            mp[tmp] = str;
        } else {
            int i = 0;
            while (i < str.size() && str[i] != ' ') {
                ++i;
            }
            int j = i;
            ++i;
            while (i < str.size()) {
                tmp1 += str[i++];
            }
            str.erase(j, i - j);
            mp[str] = tmp1;
        }
}

std::string PARSER(std::string str)
{
    char index = '"';
    std::string ret_file;
    int i = str.find(index) + 1;
    while (str[i] != '"') {
        ret_file += str[i];
        ++i;
    }
    return ret_file;
}

bool search(std::string str)
{
    str.erase(0, 7);
    for (auto it : mp) {
        if (it.first == str) {
            return true;
        }
    }
    return false;
}

bool COMENT(std::string str, bool& end)
{
    std::string s = "//";
    std::string s1 = "/*";
    std::string s2 = "*/";
    int i = 0;
    if ((i = str.find(s)) != -1) {
        str.erase(i, str.size() - i);
        end = true;
        return false;
    } else if ((i = str.find(s1)) != -1) {
        end = false;
        return false;
    } else if ((i = str.find(s2)) != -1) {
        end = true;
        return false;
    }
    return true;
}

void erase_undef(std::string str) {
    str.erase(0, 7);
    int i = 0;
    while (str[i] == ' ') {
        ++i;
    }
    str.erase(0, i);
    auto it = mp.find(str);
    mp.erase(str);
}

void file_circle(std::string fl)
{
    std::string include = "#include";
    std::string ifndef = "#ifndef";
    std::string define = "#define";
    std::string ifdef = "#ifdef";
    std::string endif = "#endif";
    std::string undef = "#undef";
    std::ifstream file;
    file.open(fl);
    std::string str;
    bool end = true;
    while (!file.eof()) {
        bool tmp = true;
        bool flag = true;
        std::getline(file, str);
        if (COMENT(str, end)) {
            if (str.find(undef) != string::npos) {
                erase_undef(str);
            } else {
                if (str.find(include) != string::npos) {
                    file_circle(PARSER(str));
                } else {
                    if (str.find(define) != string::npos && end) {
                        find_define(str);
                    } else {
                        for (auto it : mp) {
                            if (str.find(it.first) != string::npos && str.find(ifdef) == string::npos && str.find(ifndef) == string::npos && end) {
                                define_change(str, it.first, it.second);
                                flag = false;
                            }
                        }
                        if (str.find(ifndef) != string::npos) {
                            end = !(search(str));
                            flag = false;
                        }
                        if (str.find(ifdef) != string::npos && end) {
                            end = search(str);
                            tmp = false;
                        }
                        if (str.find(endif) != string::npos) {
                            end = true;
                            flag = false;
                        }
                        if (flag && end && tmp) {
                            file_o << str << '\n';
                        }
                    }
                }
            }
        }
    }
    file.close();
}

