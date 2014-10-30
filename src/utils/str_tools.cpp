#include <iostream>
#include "utils/str_tools.h"
#include "framework/exception.h"

std::string str_tools::strip(const std::string& ret)
{
    std::string str;
    size_t beg = 0;
    size_t end = 0;
    for (auto iter = ret.begin(); iter != ret.end(); ++iter) {
        if (*iter != '\n' && *iter != '\r' && *iter != '\t' && *iter != ' ')
        {
            beg = iter - ret.begin();
            break;
        }
    }
    for (auto iter = ret.end(); iter != ret.begin();) {
        --iter;
        if (*iter != '\n' && *iter != '\r' && *iter != '\t' && *iter != ' ')
        {
            end = iter - ret.begin() + 1;
            break;
        }
    }
    return  ret.substr(beg, end - beg);
}

void str_tools::split(std::string& s, char delim, std::vector<std::string>& ret)
{
    int len = s.size();
    std::string sub_str = "";
    for(int i=0; i<len; i++)
    {
        if(s.at(i) == delim)            
        {
            if(sub_str != "")
            {
                ret.push_back(sub_str);
                sub_str = "";
            }
        }
        else
            sub_str.push_back(s.at(i));
    }
    if(sub_str != "")
        ret.push_back(sub_str);
}

int str_tools::prefix_and_digits(const struct dirent *dptr)
{
    auto *p = (char *)(dptr->d_name);
    do {
        if (!std::isdigit(*p++))
            return 0;
    } while (*p != '\0');
    return 1;
}

int str_tools::prefix_and_digits_node(const struct dirent *dptr)
{
    auto *p = (char *)(dptr->d_name);
    std::string dir_prefix = "node";
    for (auto& c : dir_prefix) {
        if (*p++ != c) return 0;
    }
    do {
        if (!std::isdigit(*p++))
            return 0;
    } while (*p != '\0');
    return 1;
}

int str_tools::prefix_and_digits_vcpu(const struct dirent *dptr)
{
    auto *p = (char *)(dptr->d_name);
    std::string dir_prefix = "vcpu";
    for (auto& c : dir_prefix) {
        if (*p++ != c) return 0;
    }
    do {
        if (!std::isdigit(*p++))
            return 0;
    } while (*p != '\0');
    return 1;
}

void str_tools::get_dirs(std::string dir, std::string dir_prefix, std::vector<std::string>* node_dirs)
{
    struct dirent **namelist;
    int num_nodes;
    if (dir_prefix == "") {
        num_nodes = scandir(dir.c_str(), &namelist, prefix_and_digits, nullptr);
    } else if (dir_prefix == "node") {
        num_nodes = scandir(dir.c_str(), &namelist, prefix_and_digits_node, nullptr);
    } else if (dir_prefix == "vcpu") {
        num_nodes = scandir(dir.c_str(), &namelist, prefix_and_digits_vcpu, nullptr);
    } else {
        THROW(ScandirFailed, "Undefine dir prefix: " + dir_prefix);
    }
    if (num_nodes < 0) {
        THROW(ScandirFailed, "To scan directory: " + dir);
    } else {
        for (int i=0; i<num_nodes; ++i) {
            node_dirs->push_back(std::string(namelist[i]->d_name));
            free(namelist[i]);
        }
        free(namelist);
    }
}

