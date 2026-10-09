#include <windows.h>

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

#include "NKImage/NKImage.h"
#include "NKLogger/NkLog.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;

namespace {

// Les compteurs de memoire de Windows, charges a l'execution : pas de bibliotheque a lier.
struct Compteurs {
    DWORD cb;
    DWORD defautsDePage;
    SIZE_T pointeDeTravail;
    SIZE_T travail;
    SIZE_T pointePagePaginee;
    SIZE_T pagePaginee;
    SIZE_T pointePageNonPaginee;
    SIZE_T pageNonPaginee;
    SIZE_T engagement;
    SIZE_T pointeEngagement;
};

using FonctionCompteurs = BOOL(WINAPI *)(HANDLE, Compteurs *, DWORD);

// Memoire privee engagee par le processus, en octets (0 si la lecture echoue).
unsigned long long memoireDuProcessus() {
    static const FonctionCompteurs lire = reinterpret_cast<FonctionCompteurs>(
        reinterpret_cast<void *>(GetProcAddress(GetModuleHandleA("kernel32.dll"),
                                                "K32GetProcessMemoryInfo")));
    if (lire == nullptr) {
        return 0;
    }
    Compteurs c = {};
    c.cb = sizeof(c);
    if (!lire(GetCurrentProcess(), &c, sizeof(c))) {
        return 0;
    }
    return c.engagement;
}

unsigned long long tailleDuFichier(const char *chemin) {
    WIN32_FILE_ATTRIBUTE_DATA infos;
    if (!GetFileAttributesExA(chemin, GetFileExInfoStandard, &infos)) {
        return 0;
    }
    return (static_cast<unsigned long long>(infos.nFileSizeHigh) << 32) | infos.nFileSizeLow;
}

std::string nomDuFichier(const std::string &chemin) {
    const std::size_t coupure = chemin.find_last_of("/\\");
    return coupure == std::string::npos ? chemin : chemin.substr(coupure + 1);
}

}  // namespace

int nkmain(const NkEntryState &state) {
    if (state.args.Size() < 2) {
        logger.Error("Usage : mesure image1 image2 ... (cinq images)");
        return 1;
    }

    std::FILE *sortie = std::fopen("mesures.txt", "w");
    if (sortie == nullptr) {
        logger.Error("Impossible d'ecrire mesures.txt");
        return 1;
    }

    // On garde chaque image en vie pendant toute la mesure.
    std::vector<NkImage> images;

    // args[0] est l'identite du programme : les images commencent a 1.
    for (usize i = 1; i < state.args.Size(); ++i) {
        const std::string chemin = state.args[i].CStr();

        const unsigned long long avant = memoireDuProcessus();
        NkImage image;
        if (!image.Load(chemin.c_str())) {
            logger.Error("Chargement impossible : {0}", chemin.c_str());
            continue;
        }
        const unsigned long long apres = memoireDuProcessus();

        const unsigned long long calcul = static_cast<unsigned long long>(image.Width()) *
                                          static_cast<unsigned long long>(image.Height()) *
                                          static_cast<unsigned long long>(image.BytesPP());
        const unsigned long long mesure = apres > avant ? apres - avant : 0;
        const unsigned long long fichier = tailleDuFichier(chemin.c_str());

        logger.Info("{0} : {1} x {2}, {3} octets par pixel", nomDuFichier(chemin).c_str(),
                    image.Width(), image.Height(), image.BytesPP());

        std::fprintf(sortie, "image : %s, calcul : %llu, mesure : %llu, fichier : %llu\n",
                     nomDuFichier(chemin).c_str(), calcul, mesure, fichier);
        images.push_back(std::move(image));
    }

    std::fclose(sortie);
    return 0;
}
