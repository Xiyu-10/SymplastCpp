#include <string>
#include <symplast/utils.hpp>

#ifdef WIN32
std::wstring symplast::utils::utf8ToWide(const std::string &utf8) {
  if (utf8.empty())
    return {};
  int len = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(),
                                nullptr, 0);
  if (len <= 0)
    return {};
  std::wstring result(len, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, utf8.data(), (int)utf8.size(), result.data(),
                      len);
  return result;
}

std::filesystem::path symplast::utils::getExecutablePath() {
  std::vector<wchar_t> buffer(MAX_PATH);
  DWORD len = GetModuleFileNameW(nullptr, buffer.data(), (DWORD)buffer.size());
  while (len == buffer.size()) {
    buffer.resize(buffer.size() * 2);
    len = GetModuleFileNameW(nullptr, buffer.data(), (DWORD)buffer.size());
  }
  if (len == 0)
    return {};
  return std::filesystem::path(std::wstring(buffer.data(), len));
}
#endif