// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_COMMAND_TEMPLATE_H_
#define INCLUDED_COMMAND_TEMPLATE_H_

#include <string>
#include <tr1/unordered_map>
#include <vector>

namespace command_template {

typedef std::tr1::unordered_map<std::string, std::string> Environment;

bool ApplyTemplate(const std::string& pattern,
                   const Environment& env,
                   std::vector<std::string>& argv,
                   std::string *error);

char** StringVectorToArgv(const std::vector<std::string>& vec);

}  // namespace command_template

#endif  // INCLUDED_COMMAND_TEMPLATE_H_
