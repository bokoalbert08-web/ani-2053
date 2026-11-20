#include <cstdio>
#include <cstdlib>

#include "NkRef/NkRefBoard.h"
#include "NkRef/NkRefView.h"
#include "NkRef/NkRefFile.h"
#include "NKTime/NkChrono.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::printf("Usage : mesure <nombre_images>\n");
        return 1;
    }

    const int nombreImages = std::atoi(argv[1]);

    if (nombreImages <= 0) {
        std::printf("Le nombre d'images doit etre positif.\n");
        return 1;
    }

    constexpr int TAILLE_SOURCE = 1000;
    constexpr int REPETITIONS = 20;

    nkref::NkRefBoard board;
    nkref::NkRefView view;

    nkentseu::uint8 source[TAILLE_SOURCE];

    for (int i = 0; i < TAILLE_SOURCE; ++i) {
        source[i] = static_cast<nkentseu::uint8>(i % 256);
    }

    nkentseu::NkVector<nkref::NkRefFileImage> images;
    images.Resize(static_cast<nkentseu::usize>(nombreImages));

    for (int i = 0; i < nombreImages; ++i) {
        board.AddItem(
            {static_cast<float>(i), static_cast<float>(i)},
            100,
            80,
            nkref::NkString()
        );

        images[i].data = source;
        images[i].size = TAILLE_SOURCE;
    }

    nkentseu::NkVector<nkentseu::uint8> donnees;

    nkentseu::NkChrono chronoEcriture;

    for (int i = 0; i < REPETITIONS; ++i) {
        nkref::NkRefSerialize(
            donnees, board, images.Data(), view, false, true
        );
    }

    const double tempsEcriture =
        chronoEcriture.Elapsed().milliseconds / REPETITIONS;

    nkentseu::NkVector<nkref::NkRefLoadedItem> itemsLus;
    nkentseu::NkVector<nkref::NkRefStroke> traitsLus;
    nkref::NkRefView vueLue;
    bool themeSombre = false;
    bool grilleVisible = true;

    nkentseu::NkChrono chronoLecture;

    for (int i = 0; i < REPETITIONS; ++i) {
        if (!nkref::NkRefDeserialize(
                donnees.Data(),
                donnees.Size(),
                itemsLus,
                traitsLus,
                vueLue,
                themeSombre,
                grilleVisible)) {
            std::printf("Erreur pendant la deserialisation.\n");
            return 1;
        }
    }

    const double tempsLecture =
        chronoLecture.Elapsed().milliseconds / REPETITIONS;

    std::printf(
        "images : %d, octets : %zu, ecrire : %.6f ms, relire : %.6f ms\n",
        nombreImages,
        static_cast<size_t>(donnees.Size()),
        tempsEcriture,
        tempsLecture
    );

    return 0;
}
