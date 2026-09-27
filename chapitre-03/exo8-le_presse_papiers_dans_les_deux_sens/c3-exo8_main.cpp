#include "NKWindow/Core/NkMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKWindow/Core/NkEventSystem.h"
#include "NKWindow/Core/NkTypedEvents.h"
#include <windows.h>
#include <cctype>

using namespace nkentseu;

// NKWindow ne fournit pas d'API presse-papiers dans cet exemple (aucun
// fichier NkClipboard/NkTransfer trouve dans le depot local) : on utilise
// donc directement l'API Win32 native, comme deja fait pour MessageBoxA.

// --- Texte : lit, met en majuscules, remet -------------------------------
static bool DoTextRoundTrip(std::string& statusOut) {
    if (!OpenClipboard(nullptr)) {
        statusOut = "Echec ouverture presse-papiers (texte)";
        return false;
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (!hData) {
        CloseClipboard();
        statusOut = "Pas de texte dans le presse-papiers";
        return false;
    }

    char* pszText = static_cast<char*>(GlobalLock(hData));
    if (!pszText) {
        CloseClipboard();
        statusOut = "Echec lecture du texte";
        return false;
    }
    std::string text(pszText);
    GlobalUnlock(hData);
    CloseClipboard();

    for (char& c : text) {
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }

    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!hMem) {
        statusOut = "Echec allocation memoire (texte)";
        return false;
    }
    void* pDst = GlobalLock(hMem);
    memcpy(pDst, text.c_str(), text.size() + 1);
    GlobalUnlock(hMem);

    if (!OpenClipboard(nullptr)) {
        GlobalFree(hMem);
        statusOut = "Echec reouverture presse-papiers (texte)";
        return false;
    }
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();

    statusOut = "Texte (" + std::to_string(text.size()) + " car.) mis en MAJUSCULES et remis";
    return true;
}

// --- Image : lit, inverse les couleurs, remet -----------------------------
static bool DoImageRoundTrip(std::string& statusOut) {
    if (!OpenClipboard(nullptr)) {
        statusOut = "Echec ouverture presse-papiers (image)";
        return false;
    }

    HANDLE hData = GetClipboardData(CF_DIB);
    if (!hData) {
        CloseClipboard();
        statusOut = "Pas d'image (CF_DIB) dans le presse-papiers";
        return false;
    }

    SIZE_T size = GlobalSize(hData);
    BYTE* pSrc = static_cast<BYTE*>(GlobalLock(hData));
    if (!pSrc) {
        CloseClipboard();
        statusOut = "Echec lecture de l'image";
        return false;
    }

    BITMAPINFOHEADER bih;
    memcpy(&bih, pSrc, sizeof(BITMAPINFOHEADER));

    DWORD paletteBytes = 0;
    if (bih.biBitCount <= 8) {
        DWORD colors = bih.biClrUsed ? bih.biClrUsed : (1u << bih.biBitCount);
        paletteBytes = colors * sizeof(RGBQUAD);
    }
    DWORD pixelOffset = bih.biSize + paletteBytes;

    // On copie dans un nouveau bloc memoire a nous : on ne modifie jamais
    // directement la memoire encore partagee avec le presse-papiers.
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
    if (!hMem) {
        GlobalUnlock(hData);
        CloseClipboard();
        statusOut = "Echec allocation memoire (image)";
        return false;
    }
    BYTE* pDst = static_cast<BYTE*>(GlobalLock(hMem));
    memcpy(pDst, pSrc, size);
    GlobalUnlock(hData);
    CloseClipboard();

    if (bih.biBitCount != 24 && bih.biBitCount != 32) {
        GlobalUnlock(hMem);
        GlobalFree(hMem);
        statusOut = "Image trouvee (" + std::to_string(bih.biBitCount) +
                    " bits/pixel) mais seuls 24 et 32 bits sont geres";
        return false;
    }

    for (DWORD i = pixelOffset; i < size; ++i) {
        // Sur 32 bits, le 4e octet de chaque pixel est le canal alpha :
        // on l'ignore pour ne pas casser la transparence.
        if (bih.biBitCount == 32 && ((i - pixelOffset) % 4 == 3)) {
            continue;
        }
        pDst[i] = static_cast<BYTE>(255 - pDst[i]);
    }
    GlobalUnlock(hMem);

    if (!OpenClipboard(nullptr)) {
        GlobalFree(hMem);
        statusOut = "Echec reouverture presse-papiers (image)";
        return false;
    }
    EmptyClipboard();
    SetClipboardData(CF_DIB, hMem);
    CloseClipboard();

    statusOut = "Image (" + std::to_string(bih.biWidth) + "x" +
                std::to_string(std::abs(bih.biHeight)) + ", " +
                std::to_string(bih.biBitCount) + "bpp) couleurs inversees et remise";
    return true;
}

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title  = "c3-exo8 - T = texte, I = image";
    cfg.width  = 900;
    cfg.height = 300;

    Window window;
    if (!window.Create(cfg)) {
        MessageBoxA(nullptr, "Echec creation fenetre", "c3-exo8", MB_OK);
        return -1;
    }

    EventSystem &events = EventSystem::Instance();

    while (window.IsOpen()) {
        while (NkEvent* ev = events.PollEvent()) {

            if (ev->As<NkWindowCloseEvent>()) {
                window.Close();
            }
            else if (auto* kp = ev->As<NkKeyPressEvent>()) {
                std::string status;

                if (kp->GetKey() == NkKey::NK_T) {
                    DoTextRoundTrip(status);
                    window.SetTitle("c3-exo8 - " + status);
                }
                else if (kp->GetKey() == NkKey::NK_I) {
                    DoImageRoundTrip(status);
                    window.SetTitle("c3-exo8 - " + status);
                }
            }
        }
    }

    return 0;
}
