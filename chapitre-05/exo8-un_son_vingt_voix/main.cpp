#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    long long duree = 0;
    long long chargement = 0;
    int nombre = 0;
    std::cin >> duree >> chargement >> nombre;

    std::vector<long long> instants;
    long long lu = 0;
    for (int i = 0; i < nombre && std::cin >> lu; ++i) {
        instants.push_back(lu);
    }

    long long voixMax = 0;
    long long retardMax = 0;
    long long coupes = 0;
    long long finChargement = 0;
    std::size_t finies = 0;

    for (std::size_t i = 0; i < instants.size(); ++i) {
        const long long t = instants[i];

        // Une voix par lecture : les lectures deja lancees qui n'ont pas fini a l'instant t.
        // Une lecture qui finit exactement a t a fini (fin exclue). Les instants sont
        // tries, donc les lectures finies forment toujours un debut de liste.
        while (finies <= i && instants[finies] + duree <= t) {
            ++finies;
        }
        const long long voix = static_cast<long long>(i + 1 - finies);

        // Le fichier recharge a chaque lecture : les chargements s'enchainent, sans se
        // superposer. Le premier n'attend personne.
        const long long debut = (i == 0) ? t : std::max(t, finChargement);
        finChargement = debut + chargement;
        const long long retard = finChargement - t;

        // Une seule voix, rejouee : coupee si la suivante commence avant la fin, strictement.
        const bool coupe = i + 1 < instants.size() && instants[i + 1] < t + duree;
        if (coupe) {
            ++coupes;
        }

        std::cout << t << ' ' << voix << ' ' << retard << ' ' << (coupe ? "COUPE" : "ENTIER")
                  << '\n';
        voixMax = std::max(voixMax, voix);
        retardMax = std::max(retardMax, retard);
    }

    std::cout << "VOIX_MAX " << voixMax << '\n';
    std::cout << "RETARD_MAX " << retardMax << '\n';
    std::cout << "COUPES " << coupes << '\n';
    return 0;
}
