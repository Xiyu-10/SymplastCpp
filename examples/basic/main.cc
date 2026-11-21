#include "symplast/symplast.hpp"
#include <iostream>

#if defined(WIN32)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR pCmdLine, int nCmdShow)
#else
int main()
#endif
{
#if defined(WIN32)
  symplast::Application app({"Symplast Example"},
                            {hInstance, hPrevInstance, pCmdLine, nCmdShow});

#else
  symplast::Application app({"Symplast Example"});
#endif
  app.ready.subscribe([&]() {
    std::cout << "Ready!" << std::endl;
    app.navigate(
        "file:///" +
        (symplast::utils::getExecutablePath().parent_path() / "hello.html")
            .string());
  });
  return app.exec();
}