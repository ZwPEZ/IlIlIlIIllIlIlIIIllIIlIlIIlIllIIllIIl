#include <exception>
#include <iostream>

#include "overlay/overlay.h"

int main()
{
    try
    {
        Overlay app{};
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[Overlay] " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
