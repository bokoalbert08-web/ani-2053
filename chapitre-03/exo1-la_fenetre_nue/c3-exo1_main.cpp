#include "NKWindow/NKWindow.h"
#include "NKWindow/NKMain.h"

int nkmain(const NkEntryState &state) {
    NkWindowConfig cfg;
    NkWindow window(cfg);
    if (!window.IsOpen()) return -1;
    while (window.IsOpen()) {}
    return 0;
}
