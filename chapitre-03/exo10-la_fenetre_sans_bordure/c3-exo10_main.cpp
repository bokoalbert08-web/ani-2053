#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>
#include <chrono>

using namespace nkentseu;

// Zone de la barre de titre personnalisee (en haut de la fenetre).
static const int TITLEBAR_HEIGHT = 32;
static const int BUTTON_WIDTH    = 46;

// Dessine la barre de titre : titre a gauche, 3 boutons a droite
// (minimiser, agrandir/restaurer, fermer).
static void PaintTitleBar(HWND hwnd, NkU32 windowWidth, bool maximized)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT bar = { 0, 0, static_cast<LONG>(windowWidth), TITLEBAR_HEIGHT };
    HBRUSH barBrush = CreateSolidBrush(RGB(30, 30, 30));
    FillRect(hdc, &bar, barBrush);
    DeleteObject(barBrush);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(230, 230, 230));
    RECT titleRect = { 12, 0, static_cast<LONG>(windowWidth) - 3 * BUTTON_WIDTH, TITLEBAR_HEIGHT };
    DrawTextA(hdc, "La fenetre sans bordure", -1, &titleRect,
              DT_SINGLELINE | DT_VCENTER | DT_LEFT);

    // Les 3 boutons : minimiser, agrandir/restaurer, fermer.
    const char* labels[3] = { "_", maximized ? "=" : "o", "X" };
    for (int i = 0; i < 3; ++i)
    {
        RECT btn;
        btn.left   = static_cast<LONG>(windowWidth) - (3 - i) * BUTTON_WIDTH;
        btn.right  = btn.left + BUTTON_WIDTH;
        btn.top    = 0;
        btn.bottom = TITLEBAR_HEIGHT;
        DrawTextA(hdc, labels[i], -1, &btn, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    }

    EndPaint(hwnd, &ps);
}

// Renvoie 0 (aucun), 1 (minimiser), 2 (agrandir), 3 (fermer) selon la
// position (x, y) du clic, en pixels client.
static int HitTestButton(NkI32 x, NkI32 y, NkU32 windowWidth)
{
    if (y < 0 || y >= TITLEBAR_HEIGHT) return 0;
    NkI32 fromRight = static_cast<NkI32>(windowWidth) - x;
    if (fromRight < 0) return 0;
    int slot = fromRight / BUTTON_WIDTH;
    if (slot == 0) return 3; // fermer
    if (slot == 1) return 2; // agrandir / restaurer
    if (slot == 2) return 1; // minimiser
    return 0;
}

int nkmain(const NkEntryState &state) {
    AllocConsole();
    FILE* dummy;
    freopen_s(&dummy, "CONOUT$", "w", stdout);

    auto t0 = std::chrono::steady_clock::now();

    NkWindowConfig cfg;
    cfg.title  = "La fenetre sans bordure";
    cfg.width  = 1000;
    cfg.height = 600;
    cfg.frame  = false; // pas de bordure/barre de titre systeme

    Window window(cfg);
    if (!window.IsOpen()) {
        printf("[app] creation fenetre echouee\n");
        return -1;
    }

    HWND hwnd = window.GetSurfaceDesc().hwnd;
    bool maximized = false;

    auto t1 = std::chrono::steady_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    printf("Temps de creation de la fenetre : %.2f ms\n", elapsedMs);

    EventSystem &events = EventSystem::Instance();

    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {
            if (ev->type == NkEventType::NK_WINDOW_CLOSE) {
                window.Close();
            }
            else if (ev->type == NkEventType::NK_WINDOW_RESIZE) {
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            else if (ev->type == NkEventType::NK_MOUSE_BUTTON_PRESS) {
                NkI32 x = ev->data.mouseButton.x;
                NkI32 y = ev->data.mouseButton.y;
                NkU32 w = window.GetSize().x;

                int btn = HitTestButton(x, y, w);
                if (btn == 3) {
                    window.Close();
                }
                else if (btn == 1) {
                    window.Minimize();
                }
                else if (btn == 2) {
                    if (maximized) window.Restore();
                    else window.Maximize();
                    maximized = !maximized;
                    InvalidateRect(hwnd, nullptr, TRUE);
                }
                // Sinon (clic dans la zone titre, hors boutons) : le
                // deplacement a la souris est deja gere nativement par
                // Windows via WM_NCHITTEST -> HTCAPTION (voir la note
                // dans c3-exo10_reponse.md).
            }
        }
        PaintTitleBar(hwnd, window.GetSize().x, maximized);
    }

    return 0;
}
