#include <cmath>
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const double pi = 3.141592653589793;

    int cercles = 0;
    std::cin >> cercles;

    long long visibles = 0;
    long long refuses = 0;

    long long r = 0;
    long long n = 0;
    for (int i = 0; i < cercles && std::cin >> r >> n; ++i) {
        if (n < 3) {
            std::cout << r << ' ' << n << " REFUSE\n";
            ++refuses;
            continue;
        }

        // cos attend des radians : pi / n l'est deja.
        const double g = static_cast<double>(r) * (1.0 - std::cos(pi / static_cast<double>(n)));
        const long long ecart = static_cast<long long>(std::floor(g * 1000.0));

        if (g <= 0.0) {
            std::cout << r << ' ' << n << ' ' << ecart << " JAMAIS\n";
            continue;
        }

        const long long zoom = static_cast<long long>(std::ceil(100.0 / g));
        const bool visible = zoom <= 100;
        if (visible) {
            ++visibles;
        }
        std::cout << r << ' ' << n << ' ' << ecart << ' ' << zoom << ' ';
        std::cout << (visible ? "VISIBLE" : "INVISIBLE") << '\n';
    }

    std::cout << "VISIBLES " << visibles << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
