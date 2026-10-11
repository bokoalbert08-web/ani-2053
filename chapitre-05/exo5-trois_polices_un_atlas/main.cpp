#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Groupe {
    std::string police;
    long long n = 0;
    long long w = 0;
    long long h = 0;
};

struct Place {
    long long x1 = 0;
    long long y1 = 0;
    long long x2 = 0;
    long long y2 = 0;
};

// Un essai range tous les glyphes, groupe apres groupe, par etageres.
// Tous les glyphes d'un groupe ont la meme taille : on en pose d'un coup autant qu'il en
// tient sur l'etagere, ce qui revient exactement a les poser un par un.
bool essayer(const std::vector<Groupe> &groupes, long long marge, long long largeur,
             long long hauteur, std::vector<Place> &places) {
    places.clear();
    long long x = marge;
    long long y = marge;
    long long etagere = 0;

    for (const Groupe &groupe : groupes) {
        const long long rw = groupe.w + marge;
        const long long rh = groupe.h + marge;
        Place place;
        bool premier = true;
        long long restants = groupe.n;

        while (restants > 0) {
            if (x + rw > largeur - marge) {
                x = marge;
                y = y + etagere + marge;
                etagere = 0;
                if (x + rw > largeur - marge) {
                    return false;
                }
            }
            if (y + rh > hauteur - marge) {
                return false;
            }
            const long long tient = std::min(restants, (largeur - marge - x) / rw);
            if (premier) {
                place.x1 = x;
                place.y1 = y;
                premier = false;
            }
            place.x2 = x + (tient - 1) * rw;
            place.y2 = y;
            x += tient * rw;
            etagere = std::max(etagere, rh);
            restants -= tient;
        }
        places.push_back(place);
    }
    return true;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long marge = 0;
    long long voulue = 0;
    int nombre = 0;
    std::cin >> marge >> voulue >> nombre;

    std::vector<Groupe> groupes;
    Groupe lu;
    for (int i = 0; i < nombre && std::cin >> lu.police >> lu.n >> lu.w >> lu.h; ++i) {
        groupes.push_back(lu);
    }

    if (groupes.empty()) {
        std::cout << "AUCUN\n";
        return 0;
    }

    long long occupe = 0;
    long long besoin = 0;
    for (const Groupe &g : groupes) {
        occupe += g.n * g.w * g.h;
        besoin += g.n * (g.w + marge) * (g.h + marge);
    }

    // Largeur de depart : celle voulue, ou 512 qui double tant que le besoin n'y tient pas.
    long long largeur = voulue;
    if (voulue == 0) {
        largeur = 512;
        while (largeur * largeur < 2 * besoin && largeur < 4096) {
            largeur *= 2;
        }
    }
    long long hauteur = largeur;

    std::vector<Place> places;
    for (int essai = 1; essai <= 8; ++essai) {
        if (essayer(groupes, marge, largeur, hauteur, places)) {
            const long long surface = largeur * hauteur;
            std::cout << "ESSAIS " << essai << '\n';
            std::cout << "TEXTURE " << largeur << ' ' << hauteur << '\n';
            for (std::size_t i = 0; i < groupes.size(); ++i) {
                std::cout << groupes[i].police << ' ' << places[i].x1 << ' ' << places[i].y1 << ' ';
                std::cout << places[i].x2 << ' ' << places[i].y2 << '\n';
            }
            std::cout << "OCCUPE " << occupe << '\n';
            std::cout << "PERDU " << (surface - occupe) * 100 / surface << '\n';
            return 0;
        }
        // Un essai manque : la texture grandit, puis tout recommence.
        if (largeur == hauteur) {
            largeur *= 2;
        } else {
            hauteur = largeur;
        }
    }

    std::cout << "ESSAIS 8\n";
    std::cout << "ECHEC\n";
    return 0;
}
