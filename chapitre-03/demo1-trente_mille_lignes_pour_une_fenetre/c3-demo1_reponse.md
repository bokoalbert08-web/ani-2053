# Démonstration — Trente mille lignes pour une fenêtre

## Une. Nombre de fichiers et de lignes du module

Commande utilisée :

```bash
find /c/Users/O/Jenga/Jenga/Exemples/27_nk_window/NKWindow/src -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.mm" \) | wc -l
find /c/Users/O/Jenga/Jenga/Exemples/27_nk_window/NKWindow/src -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.mm" \) -exec cat {} + | wc -l
```

R�sultat mesuré :

- **111 fichiers** (`.cpp`, `.h`, `.mm`)
- **24 314 lignes** au total

## Deux. Liste des backends de plateforme

Commande utilisée :

```bash
ls /c/Users/O/Jenga/Jenga/Exemples/27_nk_window/NKWindow/src/NKWindow/Platform/
```

R�sultat : `Android`, `Cocoa`, `Linux`, `Noop`, `UIKit`, `UWP`, `WASM`, `Win32`, `XCB`, `XLib`

Soit **10 backends**.

## Trois. Un appel, deux implémentations

Appel choisi : **`Window::Close()`**, suivi dans les backends **Win32** et **XLib** (Linux).

**Win32** (`NkWin32WindowImpl.cpp`, ligne 230) :

```cpp
void NkWin32WindowImpl::Close()
{
    if (!mData.isOpen) return;

    NkWin32EventImpl* ev =
        static_cast<NkWin32EventImpl*>(NkGetEventImpl());
    if (ev) ev->Shutdown(mData.hwnd);

    if (mData.hwnd)
    {
        DestroyWindow(mData.hwnd);
        std::wstring wName = NkUtf8ToWide(mConfig.name);
        UnregisterClassW(wName.c_str(), mData.hinstance);
        mData.hwnd = nullptr;
    }
    if (mData.taskbarList)
    {
        // (suite non relevée)
```

**XLib** (`NkXLibWindowImpl.cpp`, ligne 81) :

```cpp
void NkXLibWindowImpl::Close()
{
    if (!mData.isOpen) return;
    if (auto* ev = static_cast<NkXLibEventImpl*>(NkGetEventImpl()))
        ev->Shutdown(&mData.window);

    if (mData.blankCursor) { XFreeCursor(mData.display,mData.blankCursor); mData.blankCursor=0; }
    if (mData.gc)          { XFreeGC(mData.display,mData.gc); mData.gc=nullptr; }
    if (mData.window)      { XDestroyWindow(mData.display,mData.window); mData.window=0; }
    XFlush(mData.display);
    mData.isOpen=false;
}
```

## Ce qui est identique, ce qui change

**Identique** : la forme du raisonnement est la même ligne pour ligne. Les deux fonctions gardent la même garde d'entrée (`if (!mData.isOpen) return;`), préviennent le système d'événements de la fermeture (`ev->Shutdown(...)`) avant de toucher au handle natif, libèrent les ressources associées à la fenêtre (police système côté Win32 dans la partie non montrée, curseur et contexte graphique côté XLib), détruisent le handle de fenêtre natif, puis mettent l'état interne à jour.

**Change** : uniquement le vocabulaire et les types de l'API native de chaque plateforme. Là où Win32 appelle `DestroyWindow` sur un `HWND` puis désenregistre une classe de fenêtre (`UnregisterClassW`), XLib appelle `XDestroyWindow` sur un `Window` (type Xlib) et libère explicitement un curseur vide et un contexte graphique (`GC`) qui n'ont pas d'équivalent direct dans l'extrait Win32 montré. XLib termine par un `XFlush` explicite (propre au modèle client/serveur de X11, où les commandes sont mises en tampon jusqu'à un envoi explicite), ce que Win32 n'a pas besoin de faire.

Ce que le module absorbe, c'est exactement cette différence que je viens de lire, en trois phrases : chaque système de fenêtrage a son propre jeu de fonctions, ses propres types de handles, et ses propres règles pour libérer une fenêtre (immédiat sous Win32, mise en tampon puis vidée sous X11). Le module fournit une seule méthode publique, `Close()`, qui cache ce vocabulaire différent derrière un nom commun. L'appelant n'a jamais besoin de savoir si, en dessous, il parle à `user32.dll` ou à un serveur X11.
