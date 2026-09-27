# Exercice — La fenêtre sans bordure

## Objectif

Ouvrir une fenêtre sans bordure système, avec une barre de titre personnalisée : le titre, trois boutons, le déplacement à la souris, et le double-clic qui agrandit. Compter le temps que cela a pris.

## Code

```cpp
#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkEventSystem.h"
#include <windows.h>
#include <cstdio>
#include <chrono>

using namespace nkentseu;

static const int TITLEBAR_HEIGHT = 32;
static const int BUTTON_WIDTH    = 46;

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
    cfg.frame  = false;

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
            }
        }
        PaintTitleBar(hwnd, window.GetSize().x, maximized);
    }

    return 0;
}
```

## Conception retenue

- **Sans bordure** : `cfg.frame = false`. D'après le code source de la bibliothèque (`NkWin32WindowImpl.cpp`), ce réglage retire `WS_OVERLAPPEDWINDOW` mais conserve `WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX` : la fenêtre garde donc ses capacités système (redimensionnement, minimiser/agrandir) sans afficher la barre de titre native.
- **Barre de titre personnalisée** : dessinée à la main avec GDI (`FillRect`, `DrawTextA`) sur les 32 premiers pixels de la fenêtre, redessinée à chaque `WM_PAINT` (déclenché ici manuellement après chaque passage de boucle et sur redimensionnement).
- **Trois boutons** : zones cliquables calculées par position (`HitTestButton`), à droite de la barre : minimiser (`_`), agrandir/restaurer (`o`/`=`), fermer (`X`). Chacun appelle la méthode correspondante de `Window` (`Minimize()`, `Maximize()`, `Restore()`, `Close()`).
- **Déplacement à la souris et double-clic agrandissant** : je n'ai pas ajouté de code spécifique pour ces deux comportements. En lisant le fichier `NkWin32EventImpl.cpp` de la bibliothèque, le message `WM_NCHITTEST` y est déjà intercepté et renvoie `HTCAPTION` pour toute la zone du haut de la fenêtre (hors les coins/bords de redimensionnement) quand `frame` est à `false` — c'est ce code, déjà présent dans la bibliothèque, qui est censé déléguer à Windows le glisser-déposer et le double-clic d'agrandissement, exactement comme une vraie barre de titre système.

## Remarque importante sur les tests effectués

La compilation a réussi sans erreur. À l'exécution, la fenêtre "La fenetre sans bordure" s'affiche avec ma barre de titre personnalisée (fond sombre, boutons visibles), mais **elle se fige immédiatement** ("ne répond pas"), sans que je puisse confirmer si le déplacement à la souris, le double-clic pour agrandir, ou les clics sur les trois boutons fonctionnent réellement. Le raisonnement ci-dessus sur `HTCAPTION` s'appuie sur la lecture du code source de la bibliothèque, pas sur une observation confirmée. Je n'ai pas non plus pu vérifier si le temps de création affiché en console (`printf`) s'est bien affiché.
