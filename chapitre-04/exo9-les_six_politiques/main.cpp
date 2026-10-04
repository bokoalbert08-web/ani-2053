#include <algorithm>
#include <iostream>

namespace {

struct Cadrage {
    long long vx = 0;
    long long vy = 0;
    long long vw = 0;
    long long vh = 0;
    long long mw = 0;
    long long mh = 0;
};

// Arrondi a l'entier le plus proche, une moitie monte : a >= 0 et b > 0.
long long arrondi(long long a, long long b) {
    return (2 * a + b) / (2 * b);
}

Cadrage centre(long long w, long long h, long long vw, long long vh, long long rw, long long rh) {
    return {(w - vw) / 2, (h - vh) / 2, vw, vh, rw, rh};
}

Cadrage letterbox(long long rw, long long rh, long long w, long long h) {
    if (w * rh <= h * rw) {
        return centre(w, h, w, arrondi(rh * w, rw), rw, rh);
    }
    return centre(w, h, arrondi(rw * h, rh), h, rw, rh);
}

Cadrage echelleEntiere(long long rw, long long rh, long long w, long long h) {
    if (w < rw || h < rh) {
        return letterbox(rw, rh, w, h);
    }
    const long long k = std::min(w / rw, h / rh);
    return centre(w, h, rw * k, rh * k, rw, rh);
}

Cadrage rogne(long long rw, long long rh, long long w, long long h) {
    if (w * rh > h * rw) {
        return {0, 0, w, h, rw, arrondi(rw * h, w)};
    }
    return {0, 0, w, h, arrondi(rh * w, h), rh};
}

}  // namespace

int main() {
    long long rw = 0;
    long long rh = 0;
    long long aw = 0;
    long long ah = 0;
    long long w = 0;
    long long h = 0;
    std::cin >> rw >> rh >> aw >> ah >> w >> h;

    // Sans reference (RW ou RH nul), quatre politiques font comme FOLLOW_WINDOW.
    const bool reference = rw > 0 && rh > 0;
    const Cadrage suivre = {0, 0, w, h, w, h};

    const char *noms[6] = {"FOLLOW_WINDOW", "STRETCH",  "FIT_LETTERBOX",
                           "INTEGER_SCALE", "FIT_CROP", "MANUAL"};
    const Cadrage politiques[6] = {
        suivre,
        reference ? Cadrage{0, 0, w, h, rw, rh} : suivre,
        reference ? letterbox(rw, rh, w, h) : suivre,
        reference ? echelleEntiere(rw, rh, w, h) : suivre,
        reference ? rogne(rw, rh, w, h) : suivre,
        Cadrage{0, 0, aw, ah, aw, ah},
    };

    int bandes = 0;
    for (int i = 0; i < 6; ++i) {
        const Cadrage &c = politiques[i];
        std::cout << noms[i] << ' ' << c.vx << ' ' << c.vy << ' ' << c.vw << ' ' << c.vh;
        std::cout << ' ' << c.mw << ' ' << c.mh << '\n';
        if (c.vw < w || c.vh < h) {
            ++bandes;
        }
    }

    std::cout << "BANDES " << bandes << '\n';
    const bool deformation = reference && w * rh != h * rw;
    std::cout << "DEFORMATION " << (deformation ? "OUI" : "NON") << '\n';
    return 0;
}
