#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkSystem.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>

using namespace nkentseu;

int nkmain(const NkEntryState &state) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    // Initialise la plateforme et attache l'EventImpl au systeme d'evenements.
    // Sans cet appel, personne ne traite les messages Windows.
    if (!NkInitialise()) {
        printf("[app] NkInitialise a echoue\n");
        return -1;
    }

    NkWindowConfig cfgA;
    cfgA.title  = "Fenetre A";
    cfgA.width  = 600;
    cfgA.height = 400;
    cfgA.x      = 100;
    cfgA.y      = 100;

    NkWindowConfig cfgB;
    cfgB.title  = "Fenetre B";
    cfgB.width  = 600;
    cfgB.height = 400;
    cfgB.x      = 800;
    cfgB.y      = 100;

    Window winA;
    Window winB;
    if (!winA.Create(cfgA) || !winB.Create(cfgB)) {
        printf("[app] creation d'une des fenetres echouee\n");
        return -1;
    }

    bool closeA = false;
    bool closeB = false;

    // Un callback par fenetre : c'est lui qui sait quelle fenetre l'a recu.
    winA.SetEventCallback([&](NkEvent* ev) {
        if (ev->type == NkEventType::NK_MOUSE_BUTTON_PRESS)
            printf("Clic recu par la fenetre A (x=%d, y=%d)\n",
                   ev->data.mouseButton.x, ev->data.mouseButton.y);
        else if (ev->type == NkEventType::NK_WINDOW_CLOSE)
            closeA = true;
    });

    winB.SetEventCallback([&](NkEvent* ev) {
        if (ev->type == NkEventType::NK_MOUSE_BUTTON_PRESS)
            printf("Clic recu par la fenetre B (x=%d, y=%d)\n",
                   ev->data.mouseButton.x, ev->data.mouseButton.y);
        else if (ev->type == NkEventType::NK_WINDOW_CLOSE)
            closeB = true;
    });

    EventSystem& events = EventSystem::Instance();
    while (winA.IsOpen() || winB.IsOpen()) {
        events.PollEvents();
        if (closeA && winA.IsOpen()) winA.Close();
        if (closeB && winB.IsOpen()) winB.Close();
        Sleep(1);
    }

    NkClose();
    return 0;
}
