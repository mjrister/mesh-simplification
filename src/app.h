#ifndef APP_H_
#define APP_H_

#include <utility>

namespace app {

void Run(const char* app_name, std::pair<int, int> window_size, std::pair<int, int> opengl_version);

}

#endif  // APP_H_
