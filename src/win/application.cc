#include <symplast/symplast.hpp>
#include <symplast/utils.hpp>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

#include <windows.h>
#include <winuser.h>
#include <wrl.h>

#include <combaseapi.h>

#include <WebView2.h>

using namespace Microsoft::WRL;

symplast::Application *instance = nullptr;

struct symplast::ApplicationPlatformData {
  WNDCLASSEXW windowClass = {};
  HWND hWnd = NULL;
  std::wstring className;
  ICoreWebView2Controller *controller = nullptr;
  ICoreWebView2 *webView = nullptr;
  std::wstring appName;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_SIZE: {
    auto platformData = symplast::Application::getInstance()->getPlatformData();
    if (platformData->controller) {
      RECT bounds;
      GetClientRect(hWnd, &bounds);
      platformData->controller->put_Bounds(bounds);
    }
    break;
  }
  case WM_DESTROY:
    symplast::Application::getInstance()->beforeQuit.emit();
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProcW(hWnd, msg, wParam, lParam);
  }
  return 0;
}

symplast::Application::Application(symplast::ApplicationOptions options,
                                   ApplicationPlatformOptions platformOptions) {
  if (instance != nullptr) {
    throw std::runtime_error(
        "'symplast::Application' should be only initialized once. If you want "
        "to "
        "get the instance, use 'symplast::Application::getInstance' instead.");
  }
  instance = this;
  this->platformOptions = platformOptions;
  platformData = std::make_unique<ApplicationPlatformData>();
#ifdef _DEBUG
  if (platformOptions.redirectStdio) {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
      FILE *fp;
      freopen_s(&fp, "CONOUT$", "w", stdout);
      freopen_s(&fp, "CONOUT$", "w", stderr);
    }
  }
#endif

  WNDCLASSEXW &wc = platformData->windowClass;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = platformOptions.hInstance;
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  platformData->className = utils::utf8ToWide(options.name);
  platformData->appName = utils::utf8ToWide(options.name);
  wc.lpszClassName = platformData->className.c_str();
  RegisterClassExW(&platformData->windowClass);
};

int symplast::Application::exec() {
  HWND hWnd = CreateWindowExW(
      0, platformData->className.c_str(), platformData->appName.c_str(),
      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, nullptr,
      nullptr, platformOptions.hInstance, nullptr);
  if (!hWnd)
    return 1;
  ShowWindow(hWnd, platformOptions.nCmdShow);
  UpdateWindow(hWnd);

  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  CreateCoreWebView2EnvironmentWithOptions(
      nullptr, nullptr, nullptr,
      Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
          [hWnd, this](HRESULT envResult,
                       ICoreWebView2Environment *env) -> HRESULT {
            if (FAILED(envResult) || !env)
              return envResult;
            env->CreateCoreWebView2Controller(
                hWnd,
                Callback<
                    ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                    [hWnd,
                     this](HRESULT ctrlResult,
                           ICoreWebView2Controller *controller) -> HRESULT {
                      if (FAILED(ctrlResult) || !controller)
                        return ctrlResult;

                      platformData->controller = controller;
                      platformData->controller->AddRef();
                      platformData->controller->get_CoreWebView2(
                          &platformData->webView);

                      RECT rc;
                      GetClientRect(hWnd, &rc);
                      platformData->controller->put_Bounds(rc);

                      ready.emit();

                      return S_OK;
                    })
                    .Get());
            return S_OK;
          })
          .Get());

  MSG msg;
  while (GetMessage(&msg, nullptr, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  quit.emit();

  CoUninitialize();

  return (int)msg.wParam;
}

symplast::Application::~Application() {}

std::shared_ptr<symplast::ApplicationPlatformData>
symplast::Application::getPlatformData() {
  return platformData;
}

void symplast::Application::navigate(std::string url) {
  platformData->webView->Navigate(utils::utf8ToWide(url).c_str());
}

symplast::Application *symplast::Application::getInstance() { return instance; }