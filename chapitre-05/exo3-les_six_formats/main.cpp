#include <iostream>
#include <string>

namespace {

struct Format {
    const char *nom;
    long long octets;
    bool couleur;
    bool transparence;
    bool flottants;
};

const Format kFormats[] = {
    {"GRAY8", 1, false, false, false},
    {"GRAY_A16", 2, false, true, false},
    {"RGB24", 3, true, false, false},
    {"RGBA32", 4, true, true, false},
    {"RGB96F", 12, true, false, true},
    {"RGBA128F", 16, true, true, true},
};

// La comparaison est exacte, majuscules comprises. nullptr : nom inconnu.
const Format *chercher(const std::string &nom) {
    for (const Format &format : kFormats) {
        if (nom == format.nom) {
            return &format;
        }
    }
    return nullptr;
}

// Les pertes, dans l'ordre : transparence, couleur, etendue. Gagner n'est pas perdre.
std::string pertes(const Format &source, const Format &cible) {
    std::string texte;
    const auto ajouter = [&texte](const char *perte) {
        if (!texte.empty()) {
            texte += '+';
        }
        texte += perte;
    };
    if (source.transparence && !cible.transparence) {
        ajouter("TRANSPARENCE");
    }
    if (source.couleur && !cible.couleur) {
        ajouter("COULEUR");
    }
    if (source.flottants && !cible.flottants) {
        ajouter("ETENDUE");
    }
    return texte.empty() ? "AUCUNE" : texte;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long w = 0;
    long long h = 0;
    int n = 0;
    std::cin >> w >> h >> n;

    long long total = 0;
    long long sansPerte = 0;
    long long refuses = 0;

    std::string nomSource;
    std::string nomCible;
    for (int i = 0; i < n && std::cin >> nomSource >> nomCible; ++i) {
        const Format *source = chercher(nomSource);
        const Format *cible = chercher(nomCible);
        if (source == nullptr || cible == nullptr) {
            std::cout << nomSource << ' ' << nomCible << " REFUSE\n";
            ++refuses;
            continue;
        }

        // La memoire vaut w * h * octets par pixel, sur 64 bits.
        const long long octetsSource = w * h * source->octets;
        const long long octetsCible = w * h * cible->octets;
        const std::string perdu = pertes(*source, *cible);

        std::cout << nomSource << ' ' << nomCible << ' ' << octetsSource << ' ' << octetsCible;
        std::cout << ' ' << perdu << '\n';

        total += octetsCible;
        if (perdu == "AUCUNE") {
            ++sansPerte;
        }
    }

    std::cout << "TOTAL " << total << '\n';
    std::cout << "SANS_PERTE " << sansPerte << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
