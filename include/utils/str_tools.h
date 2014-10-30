#ifndef _STR_TOOLS_
#define _STR_TOOLS_

#include <dirent.h>
#include <string>
#include <vector>

namespace str_tools {

std::string strip(const std::string& ret);
void split(std::string& s, char delim, std::vector<std::string>& ret);
int prefix_and_digits(const struct dirent *dptr);
int prefix_and_digits_node(const struct dirent *dptr);
int prefix_and_digits_vcpu(const struct dirent *dptr);
void get_dirs(std::string dir, std::string dir_prefix, std::vector<std::string>* node_dirs);

}

#endif
