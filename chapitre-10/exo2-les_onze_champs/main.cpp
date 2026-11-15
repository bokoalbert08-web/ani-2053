#include <iostream>
#include <string>

int main() {
    int n;
    std::cin >> n;

    int restants = 0;
    int changements = 0;

    for (int i = 1; i <= n; ++i) {
        int id;
        double x, y, echelle, rotation, opacite;
        int miroirX, miroirY;
        int largeur, hauteur, selectionne;
        std::string chemin;
        int octets;

        std::cin >> id >> x >> y >> echelle >> rotation >> opacite
                 >> miroirX >> miroirY >> largeur >> hauteur
                 >> selectionne >> chemin >> octets;

        if (octets == 0) {
            std::cout << "ITEM " << i << " SAUTE\n";
            continue;
        }

        ++restants;

        int changementsItem = 0;

        std::cout << "ITEM " << i << " " << restants;

        if (id != 0) {
            std::cout << " id";
            ++changementsItem;
        }

        if (selectionne != 0) {
            std::cout << " selected";
            ++changementsItem;
        }

        if (chemin != "-") {
            std::cout << " sourcePath";
            ++changementsItem;
        }

        if (changementsItem == 0) {
            std::cout << " AUCUN";
        }

        std::cout << '\n';

        changements += changementsItem;
    }

    std::cout << "RESTENT " << restants << '\n';
    std::cout << "CHANGES " << changements << '\n';

    return 0;
}
