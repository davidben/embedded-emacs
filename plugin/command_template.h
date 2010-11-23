#ifndef INCLUDED_COMMAND_TEMPLATE_H_
#define INCLUDED_COMMAND_TEMPLATE_H_

#include <string>
#include <tr1/unordered_map>
#include <vector>

namespace command_template {

typedef std::tr1::unordered_map<std::string, std::string> Environment;

bool applyTemplate(const std::string& pattern,
                   const Environment& env,
                   std::vector<std::string>& argv,
                   std::string *error);

char** stringVectorToArgv(const std::vector<std::string>& vec);

}  // namespace command_template

#endif  // INCLUDED_COMMAND_TEMPLATE_H_
