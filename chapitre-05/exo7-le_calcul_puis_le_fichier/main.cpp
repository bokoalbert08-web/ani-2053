#include <iostream>
#include <string>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long seuil = 0;
    int nombre = 0;
    std::cin >> seuil >> nombre;

    long long memoire = 0;
    long long flux = 0;
    long long refuses = 0;

    std::string nom;
    long long frequence = 0;
    long long canaux = 0;
    long long bits = 0;
    long long duree = 0;
    long long fichier = 0;
    for (int i = 0; i < nombre &&
                    std::cin >> nom >> frequence >> canaux >> bits >> duree >> fichier;
         ++i) {
        if (bits != 8 && bits != 16 && bits != 24 && bits != 32) {
            std::cout << nom << " REFUSE\n";
            ++refuses;
            continue;
        }

        // Les trois multiplications d'abord, la division par 1000 a la fin, sur 64 bits.
        const long long brut = frequence * canaux * (bits / 8) * duree / 1000;
        const long long pourcent = brut > 0 ? fichier * 100 / brut : 0;

        // Strictement plus grand que le seuil : en flux. Egal : reste en memoire.
        if (brut > seuil) {
            std::cout << nom << ' ' << brut << ' ' << pourcent << " FLUX\n";
            ++flux;
        } else {
            std::cout << nom << ' ' << brut << ' ' << pourcent << " MEMOIRE\n";
            memoire += brut;
        }
    }

    std::cout << "MEMOIRE " << memoire << '\n';
    std::cout << "FLUX " << flux << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
