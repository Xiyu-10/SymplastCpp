#pragma once

#include "export.hpp"
#include "utils.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#ifdef WIN32
#include <Windows.h>
#endif

namespace symplast {

template <class... Args> class Event {
  std::unordered_map<int, std::function<void(Args...)>> listeners;
  int id = 0;

public:
  int subscribe(std::function<void(Args...)> f) {
    id++;
    listeners[id] = f;
    return id;
  }
  void unsubscribe(int id) { listeners.erase(id); }
  void emit(Args... args) const {
    for (const auto &pair : listeners) {
      pair.second(args...);
    }
  }
};

struct ApplicationOptions {
  std::string name;
  std::string version = "0.0.0";
  std::optional<std::string> icon = std::nullopt;
};

#ifdef WIN32
struct ApplicationPlatformOptions {
  HINSTANCE hInstance;
  HINSTANCE hPrevInstance;
  PWSTR pCmdLine;
  int nCmdShow;
  bool redirectStdio = true;
};

struct ApplicationPlatformData;
#else
struct ApplicationPlatformOptions {};
#endif

class SYMPLAST_EXPORT Application {
private:
  std::shared_ptr<ApplicationPlatformData> platformData;
  ApplicationPlatformOptions platformOptions;

public:
  Event<> ready;
  Event<> quit;
  Event<> beforeQuit;
  SYMPLAST_EXPORT int exec();
  SYMPLAST_EXPORT void navigate(std::string url);
  static SYMPLAST_EXPORT Application *getInstance();
  SYMPLAST_EXPORT std::shared_ptr<ApplicationPlatformData> getPlatformData();
#ifdef WIN32
  SYMPLAST_EXPORT Application(ApplicationOptions options,
                              ApplicationPlatformOptions platformOptions);
#endif
  SYMPLAST_EXPORT ~Application();
};

}; // namespace symplast