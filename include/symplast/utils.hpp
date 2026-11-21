#include "symplast/export.hpp"
#include <filesystem>
#include <string>
#include <windows.h>

namespace symplast::utils {
#ifdef _WIN32
SYMPLAST_EXPORT std::wstring utf8ToWide(const std::string &utf8);
#endif
std::filesystem::path getExecutablePath();
} // namespace symplast::utils