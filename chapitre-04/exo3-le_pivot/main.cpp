#include <algorithm>
#include <iostream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n = 0;
    std::cin >> n;

    long long refuses = 0;

    std::string nom;
    long long w = 0;
    long long h = 0;
    long long px = 0;
    long long py = 0;
    long long ox = 0;
    long long oy = 0;
    long long sx = 0;
    long long sy = 0;
    long long angle = 0;

    for (int i = 0; i < n && std::cin >> nom >> w >> h >> px >> py >> ox >> oy >> sx >> sy >> angle; ++i) {
        if (angle % 90 != 0) {
            std::cout << nom << " ANGLE REFUSE\n";
            ++refuses;
            continue;
        }

        // En C++, -90 % 360 vaut -90 : on ramene le reste dans les positifs.
        const long long quart = (((angle % 360) + 360) % 360) / 90;
        const long long c = (quart == 0) ? 1 : (quart == 2) ? -1 : 0;
        const long long s = (quart == 1) ? 1 : (quart == 3) ? -1 : 0;

        // haut-gauche, haut-droit, bas-droit, bas-gauche
        const long long locaux[4][2] = {{0, 0}, {w, 0}, {w, h}, {0, h}};
        long long x[4];
        long long y[4];
        for (int k = 0; k < 4; ++k) {
            const long long ax = (locaux[k][0] - ox) * sx;
            const long long ay = (locaux[k][1] - oy) * sy;
            x[k] = px + ax * c - ay * s;
            y[k] = py + ax * s + ay * c;
        }

        std::cout << nom << " COINS";
        for (int k = 0; k < 4; ++k) {
            std::cout << ' ' << x[k] << ' ' << y[k];
        }
        std::cout << '\n';

        std::cout << nom << " BOITE " << *std::min_element(x, x + 4) << ' ' << *std::min_element(y, y + 4)
                  << ' ' << *std::max_element(x, x + 4) << ' ' << *std::max_element(y, y + 4) << '\n';
    }

    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
