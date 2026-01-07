#include "overlay/overlay.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Overlay overlay;
    overlay.Run();
    return 0;
}
