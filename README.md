# C++ Logging Library

## Features
With this *GREAT* library you can (but are by no means limited to):
-  receiving messages from various components of your app — yes, even from that one chaotic microservice everyone avoids
-  filtering them by all sorts of criteria — because nobody needs all the logs, right?
-  enriching them with metadata — because raw data is so last season
-  formatting them into various textual representations — JSON, plain text, Shakespearean sonnet, you name it
-  writing them to different destinations — consoles, files, Slack, your ex's email (okay, maybe not that last one)

## Dependencies
- C++20 compiler
- nlohmann/json — automatically downloaded

## Build&Run
**Build Example**
``` bash
cmake -B build -DBUILD_TESTING=OFF -DBUILD_EXAMPLE=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
**Run Example**
``` bash
./build/example/example
```
``` cpp
#include "core/log_manager.h"

int main() {
  std::string seviceName = "ipv4filter";
  auto logger = LogManager::builder()
                    .named("logger")
                    .withLevelFilter(LogLevel::TRACE)
                    .withNameFilter({seviceName})
                    .withTimestamp()
                    .withConsoleSink()
                    .build();

  LogRecord record;
  record.loggerName = seviceName;
  record.level = LogLevel::TRACE;
  record.message = "Rejected";
  record.fields = {{"IPAddress", "192.168.88.10"},
                   {"chain", "input"},
                   {"port", "53"},
                   {"protocol", "17"}};

  logger->log(record);
}
```
**Expected Output:**
``` bash
[2026-06-30 11:47:40] [TRACE] [ipv4filter] Rejected IPAddress=192.168.88.10 chain=input port=53 protocol=17
```

## Install
**Build Release shared**
``` bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF && cmake --build build -j$(nproc)
```
**Install to local dir**
``` bash
cmake --install build --prefix ./install
```

## Using in Another Project
``` cmake
# In your project's CMakeLists.txt
set(CMAKE_PREFIX_PATH "/path/to/install")

find_package(Logger REQUIRED CONFIG)

target_link_libraries(my_app PRIVATE Logger::logger)
```
``` cpp
// In your source code
#include "core/log_manager.h"
```