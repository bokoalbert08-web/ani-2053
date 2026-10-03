#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long colonnes = 0;
    long long lignes = 0;
    long long largeur = 0;
    long long hauteur = 0;
    long long cases = 0;
    long long duree = 0;
    long long plafond = 0;
    std::cin >> colonnes >> lignes >> largeur >> hauteur >> cases >> duree >> plafond;

    long long images = 0;
    std::cin >> images;

    long long courante = 0;
    long long accumule = 0;
    long long avances = 0;
    long long plafonnes = 0;

    long long dt = 0;
    for (long long i = 0; i < images && std::cin >> dt; ++i) {
        // Au retour d'une mise en veille, l'horloge rend plusieurs secondes d'un coup.
        if (dt > plafond) {
            dt = plafond;
            ++plafonnes;
        }
        accumule += dt;

        // On retire seulement la duree d'une case par passage : le reste est garde.
        const long long passages = accumule / duree;
        accumule %= duree;
        avances += passages;
        courante = (courante + passages) % cases;

        const long long x = (courante % colonnes) * largeur;
        const long long y = (courante / colonnes) * hauteur;
        std::cout << courante << ' ' << x << ' ' << y << ' ';
        std::cout << largeur << ' ' << hauteur << '\n';
    }

    std::cout << "AVANCES " << avances << '\n';
    std::cout << "PLAFONNES " << plafonnes << '\n';
    return 0;
}
