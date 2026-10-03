#include <iostream>
#include <string>

namespace {

enum Touche { ESPACE = 0, GAUCHE = 1, DROITE = 2, AUTRE = 3 };

Touche touche(const std::string &nom) {
    if (nom == "SPACE") {
        return ESPACE;
    }
    if (nom == "LEFT") {
        return GAUCHE;
    }
    if (nom == "RIGHT") {
        return DROITE;
    }
    return AUTRE;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long v = 0;
    int images = 0;
    std::cin >> v >> images;

    bool enfoncee[3] = {false, false, false};
    long long xe = 0;
    long long xi = 0;
    long long sautsEvenements = 0;
    long long sautsInterrogation = 0;
    long long manques = 0;

    int k = 0;
    for (int i = 1; i <= images && std::cin >> k; ++i) {
        long long appuisEspace = 0;

        // 1. Les evenements, un par un : chacun met a jour l'etat de sa touche.
        for (int j = 0; j < k; ++j) {
            std::string ev;
            std::cin >> ev;
            if (ev.size() < 2 || (ev[0] != '+' && ev[0] != '-')) {
                continue;
            }
            const Touche t = touche(ev.substr(1));
            if (t == AUTRE) {
                continue;
            }
            const bool appui = ev[0] == '+';
            enfoncee[t] = appui;
            if (!appui) {
                continue;
            }
            if (t == ESPACE) {
                ++sautsEvenements;
                ++appuisEspace;
            } else if (t == DROITE) {
                xe += v;
            } else {
                xe -= v;
            }
        }

        // 2. L'interrogation, une seule fois, apres tous les evenements de l'image.
        if (enfoncee[ESPACE]) {
            ++sautsInterrogation;
        } else {
            manques += appuisEspace;
        }
        if (enfoncee[DROITE]) {
            xi += v;
        }
        if (enfoncee[GAUCHE]) {
            xi -= v;
        }

        std::cout << i << ' ' << xe << ' ' << xi << '\n';
    }

    std::cout << "SAUTS EVENEMENTS " << sautsEvenements << '\n';
    std::cout << "SAUTS INTERROGATION " << sautsInterrogation << '\n';
    std::cout << "MANQUES " << manques << '\n';
    return 0;
}
