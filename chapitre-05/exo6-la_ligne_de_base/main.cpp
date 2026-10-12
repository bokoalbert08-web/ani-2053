#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {

struct Glyphe {
    long long avance = 0;
    long long x0 = 0;
    long long y0 = 0;
    long long x1 = 0;
    long long y1 = 0;
};

// Decoupe un texte en caracteres : un caractere UTF-8 peut tenir sur plusieurs octets.
std::vector<std::string> caracteres(const std::string &texte) {
    std::vector<std::string> resultat;
    std::size_t i = 0;
    while (i < texte.size()) {
        const unsigned char octet = static_cast<unsigned char>(texte[i]);
        std::size_t longueur = 1;
        if ((octet & 0xE0) == 0xC0) {
            longueur = 2;
        } else if ((octet & 0xF0) == 0xE0) {
            longueur = 3;
        } else if ((octet & 0xF8) == 0xF0) {
            longueur = 4;
        }
        longueur = std::min(longueur, texte.size() - i);
        resultat.push_back(texte.substr(i, longueur));
        i += longueur;
    }
    return resultat;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nombreGlyphes = 0;
    std::cin >> nombreGlyphes;
    std::map<std::string, Glyphe> table;
    for (int i = 0; i < nombreGlyphes; ++i) {
        std::string c;
        Glyphe g;
        std::cin >> c >> g.avance >> g.x0 >> g.y0 >> g.x1 >> g.y1;
        table[c] = g;
    }

    int nombrePaires = 0;
    std::cin >> nombrePaires;
    std::map<std::string, long long> crenage;
    for (int i = 0; i < nombrePaires; ++i) {
        std::string paire;
        long long k = 0;
        std::cin >> paire >> k;
        crenage[paire] = k;
    }

    std::string texte;
    long long ox = 0;
    long long oy = 0;
    std::cin >> texte >> ox >> oy;

    long long x = ox;
    long long absents = 0;
    bool dessine = false;
    long long minx = 0;
    long long miny = 0;
    long long maxx = 0;
    long long maxy = 0;

    const std::vector<std::string> lettres = caracteres(texte);
    for (std::size_t i = 0; i < lettres.size(); ++i) {
        const auto trouve = table.find(lettres[i]);
        if (trouve == table.end()) {
            // Absent : il n'avance pas le curseur, et aucun crenage ne se cherche apres lui.
            std::cout << lettres[i] << " ABSENT\n";
            ++absents;
            continue;
        }

        const Glyphe &g = trouve->second;
        std::cout << lettres[i] << ' ' << x << '\n';

        // Dessine seulement si son rectangle n'est pas vide : l'espace avance sans dessiner.
        if (g.x1 > g.x0 && g.y1 > g.y0) {
            const long long gauche = x + g.x0;
            const long long haut = oy + g.y0;
            const long long droite = x + g.x1;
            const long long bas = oy + g.y1;
            if (!dessine) {
                minx = gauche;
                miny = haut;
                maxx = droite;
                maxy = bas;
                dessine = true;
            } else {
                minx = std::min(minx, gauche);
                miny = std::min(miny, haut);
                maxx = std::max(maxx, droite);
                maxy = std::max(maxy, bas);
            }
        }

        x += g.avance;
        if (i + 1 < lettres.size()) {
            const auto paire = crenage.find(lettres[i] + lettres[i + 1]);
            if (paire != crenage.end()) {
                x += paire->second;
            }
        }
    }

    std::cout << "CURSEUR " << x << '\n';
    if (!dessine) {
        std::cout << "BOITE AUCUNE\n";
        std::cout << "MONTE 0\n";
        std::cout << "DESCEND 0\n";
        std::cout << "ECRAN RIEN\n";
    } else {
        std::cout << "BOITE " << minx << ' ' << miny << ' ' << maxx << ' ' << maxy << '\n';
        std::cout << "MONTE " << (miny < oy ? oy - miny : 0) << '\n';
        std::cout << "DESCEND " << (maxy > oy ? maxy - oy : 0) << '\n';
        const char *verdict = "VISIBLE";
        if (maxy <= 0) {
            verdict = "HORS";
        } else if (miny < 0) {
            verdict = "COUPE";
        }
        std::cout << "ECRAN " << verdict << '\n';
    }
    std::cout << "ABSENTS " << absents << '\n';
    return 0;
}
