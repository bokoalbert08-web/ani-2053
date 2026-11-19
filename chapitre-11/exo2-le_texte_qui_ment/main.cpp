#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

struct Commit {
    string hash;
    string action;
    vector<string> fonctions;
};

struct Annonce {
    string hash;
    string etat;
    string fonction;
};

int main() {
    int h;
    cin >> h;

    vector<Commit> commits(h);
    vector<set<string>> etats(h);

    for (int i = 0; i < h; ++i) {
        int k;
        cin >> commits[i].hash >> commits[i].action >> k;

        for (int j = 0; j < k; ++j) {
            string fonction;
            cin >> fonction;
            commits[i].fonctions.push_back(fonction);
        }

        if (i > 0) {
            etats[i] = etats[i - 1];
        }

        for (const string& fonction : commits[i].fonctions) {
            if (commits[i].action == "AJOUTE") {
                etats[i].insert(fonction);
            } else {
                etats[i].erase(fonction);
            }
        }
    }

    int a;
    cin >> a;

    vector<Annonce> annonces(a);

    for (int i = 0; i < a; ++i) {
        cin >> annonces[i].hash
            >> annonces[i].etat
            >> annonces[i].fonction;
    }

    int mensonges = 0;

    for (int i = 0; i < a; ++i) {
        int ecriture = 0;

        for (int j = 0; j < h; ++j) {
            if (commits[j].hash == annonces[i].hash) {
                ecriture = j;
                break;
            }
        }

        auto estVraie = [&](int indice) {
            bool presente = etats[indice].count(annonces[i].fonction);

            if (annonces[i].etat == "A_VENIR") {
                return !presente;
            }

            return presente;
        };

        if (estVraie(h - 1)) {
            cout << "ANNONCE " << i + 1 << " VRAI\n";
        } else {
            int debut = h - 1;

            while (debut > ecriture && !estVraie(debut - 1)) {
                --debut;
            }

            cout << "ANNONCE " << i + 1
                 << " MENT " << commits[debut].hash << '\n';

            ++mensonges;
        }
    }

    cout << "MENSONGES " << mensonges << '\n';

    return 0;
}

