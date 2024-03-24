#include "defines.hpp"
#include "application.hpp"

#ifdef PS_TOOLSET_MSVC
#define NOGDI
#define NOUSER
#include <Windows.h>
#undef near
#undef far

i32 WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    Application app;
    i32 result = app.run();
    return result;
}
#else
i32 main() {
    Application app;
    i32 result = app.run();
    return result;
}
#endif
