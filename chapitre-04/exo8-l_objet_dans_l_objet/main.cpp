#include <iostream>
#include <map>
#include <string>

namespace {

struct Objet {
    long long x = 0;
    long long y = 0;
    long long angle = 0;
    long long echelle = 1;
    long long niveau = 1;
};

long long ramener(long long angle) {
    return ((angle % 360) + 360) % 360;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n = 0;
    std::cin >> n;

    std::map<std::string, Objet> objets;
    long long profondeur = 0;

    std::string nom;
    std::string parent;
    long long tx = 0;
    long long ty = 0;
    long long angle = 0;
    long long echelle = 0;
    for (int i = 0; i < n && std::cin >> nom >> parent >> tx >> ty >> angle >> echelle; ++i) {
        Objet o;
        const auto trouve = objets.find(parent);

        if (parent == "-" || trouve == objets.end()) {
            // Sans parent : l'objet est a sa place.
            o.x = tx;
            o.y = ty;
            o.angle = ramener(angle);
            o.echelle = echelle;
            o.niveau = 1;
        } else {
            const Objet &p = trouve->second;

            // 1. L'echelle du parent dans le monde, d'abord.
            const long long ax = tx * p.echelle;
            const long long ay = ty * p.echelle;

            // 2. Puis la rotation du parent dans le monde.
            const long long quart = p.angle / 90;
            const long long c = (quart == 0) ? 1 : (quart == 2) ? -1 : 0;
            const long long s = (quart == 1) ? 1 : (quart == 3) ? -1 : 0;
            const long long rx = ax * c - ay * s;
            const long long ry = ax * s + ay * c;

            // 3. Enfin la position du parent dans le monde.
            o.x = p.x + rx;
            o.y = p.y + ry;
            o.angle = ramener(p.angle + angle);
            o.echelle = p.echelle * echelle;
            o.niveau = p.niveau + 1;
        }

        objets[nom] = o;
        if (o.niveau > profondeur) {
            profondeur = o.niveau;
        }
        std::cout << nom << ' ' << o.x << ' ' << o.y << ' ' << o.angle << ' ' << o.echelle << '\n';
    }

    std::cout << "PROFONDEUR " << profondeur << '\n';
    return 0;
}
