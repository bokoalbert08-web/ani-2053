#include <cstdio>
#include <string>

#include "NKFont/NkFont.h"
#include "NKImage/NKImage.h"
#include "NKWindow/NKMain.h"

using namespace nkentseu;

namespace {

const int kTailles[2] = {18, 48};

// Part de la texture, en pour cent, ou aucun glyphe n'est dessine : les pixels d'alpha nul.
double pourcentVide(const nkft_uint8 *pixels, int largeur, int hauteur) {
    const long long total = static_cast<long long>(largeur) * hauteur;
    long long vides = 0;
    for (long long i = 0; i < total; ++i) {
        if (pixels[i * 4 + 3] == 0) {
            ++vides;
        }
    }
    return 100.0 * static_cast<double>(vides) / static_cast<double>(total);
}

// Part de la texture, en pour cent, hors des rectangles des glyphes (marges comprises).
double pourcentHorsRectangles(const NkFont &police, int largeur, int hauteur) {
    double utile = 0.0;
    for (nkft_uint32 i = 0; i < police.glyphCount; ++i) {
        const NkFontGlyph &g = police.glyphs[i];
        utile += static_cast<double>((g.u1 - g.u0) * largeur) * ((g.v1 - g.v0) * hauteur);
    }
    return 100.0 * (1.0 - utile / (static_cast<double>(largeur) * hauteur));
}

// Un atlas est blanc sur transparent : invisible dans beaucoup de visionneuses.
// On pose donc les pixels sur un fond sombre opaque, pour que les glyphes se lisent.
void poserSurFondSombre(nkft_uint8 *pixels, int largeur, int hauteur) {
    const int fond[3] = {20, 20, 28};
    const long long total = static_cast<long long>(largeur) * hauteur;
    for (long long i = 0; i < total; ++i) {
        nkft_uint8 *p = pixels + i * 4;
        const int alpha = p[3];
        for (int c = 0; c < 3; ++c) {
            p[c] = static_cast<nkft_uint8>((255 * alpha + fond[c] * (255 - alpha)) / 255);
        }
        p[3] = 255;
    }
}

}  // namespace

int nkmain(const NkEntryState &state) {
    const std::string chemin =
        state.args.Size() > 1 ? state.args[1].CStr() : "C:/Windows/Fonts/arial.ttf";

    std::FILE *compte = std::fopen("compte.txt", "w");
    if (compte == nullptr) {
        std::printf("Impossible d'ecrire compte.txt\n");
        return 1;
    }

    for (const int taille : kTailles) {
        // Une police, une taille, un atlas : Build() rasterise tout d'un coup.
        // Sans configuration explicite, ce kit rasterise une texture vide (mesure : 0 pixel non
        // nul, contre environ 19000 avec une configuration). On la pose donc ici.
        NkFontConfig config;
        config.sizePixels = static_cast<float>(taille);
        config.glyphRanges = NkFontAtlas::GetGlyphRangesDefault();

        NkFontAtlas atlas;
        NkFont *police =
            atlas.AddFontFromFile(chemin.c_str(), static_cast<float>(taille), &config);
        if (police == nullptr || !atlas.Build()) {
            std::printf("Echec avec la police %s a la taille %d\n", chemin.c_str(), taille);
            std::fclose(compte);
            return 1;
        }

        nkft_uint8 *pixels = nullptr;
        nkft_int32 largeur = 0;
        nkft_int32 hauteur = 0;
        atlas.GetTexDataAsRGBA32(&pixels, &largeur, &hauteur);

        const double vide = pourcentVide(pixels, largeur, hauteur);
        const double hors = pourcentHorsRectangles(*police, largeur, hauteur);
        const unsigned glyphes = static_cast<unsigned>(police->glyphCount);

        std::fprintf(compte, "taille : %d, glyphes : %u, texture : %d %d, perdu : %d\n", taille,
                     glyphes, largeur, hauteur, static_cast<int>(vide + 0.5));
        std::printf("taille %d : %u glyphes, texture %d x %d, pixels vides %.2f %%, ", taille,
                    glyphes, largeur, hauteur, vide);
        std::printf("hors des rectangles %.2f %%\n", hors);

        // Seule la premiere taille est rendue ; la seconde n'est que comptee.
        if (taille == kTailles[0]) {
            poserSurFondSombre(pixels, largeur, hauteur);
            // Wrap ne copie pas les pixels : l'atlas reste en vie jusqu'a la fin de SavePNG.
            const NkImage image =
                NkImage::Wrap(pixels, largeur, hauteur, NkImagePixelFormat::NK_RGBA32);
            if (!image.SavePNG("atlas.png")) {
                std::printf("Echec de l'enregistrement de atlas.png\n");
            }
        }
    }

    std::fclose(compte);
    return 0;
}
