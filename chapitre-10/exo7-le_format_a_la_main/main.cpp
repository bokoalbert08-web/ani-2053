#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Lecteur {
private:
    const vector<uint8_t>& octets;
    size_t position = 0;

public:
    explicit Lecteur(const vector<uint8_t>& donnees) : octets(donnees) {}

    bool lireU8(uint8_t& valeur) {
        if (position >= octets.size()) {
            return false;
        }

        valeur = octets[position++];
        return true;
    }

    bool lireU32(uint32_t& valeur) {
        if (octets.size() - position < 4) {
            return false;
        }

        valeur = static_cast<uint32_t>(octets[position])
               | (static_cast<uint32_t>(octets[position + 1]) << 8)
               | (static_cast<uint32_t>(octets[position + 2]) << 16)
               | (static_cast<uint32_t>(octets[position + 3]) << 24);

        position += 4;
        return true;
    }

    bool lireF32(float& valeur) {
        uint32_t bits;

        if (!lireU32(bits)) {
            return false;
        }

        memcpy(&valeur, &bits, sizeof(valeur));
        return true;
    }

    bool sauter(size_t nombre) {
        if (nombre > octets.size() - position) {
            return false;
        }

        position += nombre;
        return true;
    }
};

string nombre(float valeur) {
    float arrondi = round(valeur * 100.0f) / 100.0f;

    if (arrondi == 0.0f) {
        arrondi = 0.0f;
    }

    ostringstream sortie;
    sortie << fixed << setprecision(2) << arrondi;
    return sortie.str();
}

int main() {
    size_t total;

    if (!(cin >> total)) {
        cout << "TRONQUE\n";
        return 0;
    }

    vector<uint8_t> octets;
    octets.reserve(total);

    for (size_t i = 0; i < total; ++i) {
        string hexadecimal;

        if (!(cin >> hexadecimal)) {
            break;
        }

        octets.push_back(static_cast<uint8_t>(
            stoul(hexadecimal, nullptr, 16)
        ));
    }

    if (octets.size() < 4) {
        cout << "TRONQUE\n";
        return 0;
    }

    const uint8_t signature[4] = {0x4E, 0x4B, 0x52, 0x46};

    for (int i = 0; i < 4; ++i) {
        if (octets[i] != signature[i]) {
            cout << "PAS_UNE_PLANCHE\n";
            return 0;
        }
    }

    Lecteur lecteur(octets);
    uint8_t ignore;
    uint32_t version;

    lecteur.sauter(4);

    if (!lecteur.lireU32(version)) {
        cout << "TRONQUE\n";
        return 0;
    }

    if (version != 1) {
        cout << "VERSION_INCONNUE " << version << "\n";
        return 0;
    }

    ostringstream resultat;

    float cx, cy, zoom;
    uint8_t theme, grille;

    if (!lecteur.lireF32(cx) ||
        !lecteur.lireF32(cy) ||
        !lecteur.lireF32(zoom) ||
        !lecteur.lireU8(theme) ||
        !lecteur.lireU8(grille) ||
        !lecteur.lireU8(ignore) ||
        !lecteur.lireU8(ignore)) {
        cout << "TRONQUE\n";
        return 0;
    }

    resultat << "VUE " << nombre(cx) << " " << nombre(cy)
             << " " << nombre(zoom) << "\n";
    resultat << "THEME " << (theme != 0 ? "SOMBRE" : "CLAIR") << "\n";
    resultat << "GRILLE " << (grille == 0 ? "AFFICHEE" : "CACHEE") << "\n";

    uint32_t nombreImages;

    if (!lecteur.lireU32(nombreImages)) {
        cout << "TRONQUE\n";
        return 0;
    }

    resultat << "IMAGES " << nombreImages << "\n";

    for (uint64_t i = 1; i <= nombreImages; ++i) {
        float x, y, echelle, rotation, opacite;
        uint8_t miroirX, miroirY, remplissage;
        uint32_t largeur, hauteur, taille;

        if (!lecteur.lireF32(x) ||
            !lecteur.lireF32(y) ||
            !lecteur.lireF32(echelle) ||
            !lecteur.lireF32(rotation) ||
            !lecteur.lireF32(opacite) ||
            !lecteur.lireU8(miroirX) ||
            !lecteur.lireU8(miroirY) ||
            !lecteur.lireU8(remplissage) ||
            !lecteur.lireU8(remplissage) ||
            !lecteur.lireU32(largeur) ||
            !lecteur.lireU32(hauteur) ||
            !lecteur.lireU32(taille) ||
            !lecteur.sauter(taille)) {
            cout << "TRONQUE\n";
            return 0;
        }

        resultat << "IMAGE " << i << " "
                 << nombre(x) << " " << nombre(y) << " "
                 << nombre(echelle) << " " << nombre(rotation) << " "
                 << nombre(opacite) << " "
                 << (miroirX != 0 ? 1 : 0) << " "
                 << (miroirY != 0 ? 1 : 0) << " "
                 << largeur << " " << hauteur << " " << taille << "\n";
    }

    uint32_t nombreTraits;

    if (!lecteur.lireU32(nombreTraits)) {
        cout << "TRONQUE\n";
        return 0;
    }

    resultat << "TRAITS " << nombreTraits << "\n";

    for (uint64_t i = 1; i <= nombreTraits; ++i) {
        uint8_t r, g, b, a;
        float epaisseur;
        uint32_t nombrePoints;

        if (!lecteur.lireU8(r) ||
            !lecteur.lireU8(g) ||
            !lecteur.lireU8(b) ||
            !lecteur.lireU8(a) ||
            !lecteur.lireF32(epaisseur) ||
            !lecteur.lireU32(nombrePoints)) {
            cout << "TRONQUE\n";
            return 0;
        }

        resultat << "TRAIT " << i << " "
                 << static_cast<int>(r) << " "
                 << static_cast<int>(g) << " "
                 << static_cast<int>(b) << " "
                 << static_cast<int>(a) << " "
                 << nombre(epaisseur) << " "
                 << nombrePoints << "\n";

        for (uint32_t j = 0; j < nombrePoints; ++j) {
            float px, py;

            if (!lecteur.lireF32(px) || !lecteur.lireF32(py)) {
                cout << "TRONQUE\n";
                return 0;
            }

            resultat << "POINT " << nombre(px) << " "
                     << nombre(py) << "\n";
        }
    }

    cout << resultat.str();
    return 0;
}
