#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Methode {
    string nom;
    bool estConst;
    bool modifie;
    vector<string> appels;
};

int trouverMethode(const vector<Methode>& methodes, const string& nom) {
    for (int i = 0; i < (int)methodes.size(); i++) {
        if (methodes[i].nom == nom) {
            return i;
        }
    }

    return -1;
}

int main() {
    int n;
    cin >> n;

    vector<Methode> methodes(n);

    for (int i = 0; i < n; i++) {
        string declaration;
        int modifie;
        int k;

        cin >> methodes[i].nom >> declaration >> modifie >> k;

        methodes[i].estConst = (declaration == "const");
        methodes[i].modifie = (modifie == 1);

        for (int j = 0; j < k; j++) {
            string appel;
            cin >> appel;
            methodes[i].appels.push_back(appel);
        }
    }

    vector<bool> pourrait(n, false);

    // Première sélection : les méthodes qui ne modifient rien.
    for (int i = 0; i < n; i++) {
        pourrait[i] = !methodes[i].estConst && !methodes[i].modifie;
    }

    // Éliminer les méthodes qui appellent une méthode impossible à const.
    bool changement = true;

    while (changement) {
        changement = false;

        for (int i = 0; i < n; i++) {
            if (!pourrait[i]) {
                continue;
            }

            for (const string& nomAppel : methodes[i].appels) {
                int indice = trouverMethode(methodes, nomAppel);

                if (!methodes[indice].estConst && !pourrait[indice]) {
                    pourrait[i] = false;
                    changement = true;
                    break;
                }
            }
        }
    }

    // Afficher les méthodes qui pourraient devenir const.
    cout << "POURRAIT";
    for (int i = 0; i < n; i++) {
        if (pourrait[i]) {
            cout << " " << methodes[i].nom;
        }
    }

    bool aucunPourrait = true;
    for (int i = 0; i < n; i++) {
        if (pourrait[i]) {
            aucunPourrait = false;
            break;
        }
    }

    if (aucunPourrait) {
        cout << " AUCUN";
    }
    cout << '\n';

    // Identifier les racines.
    cout << "RACINES";
    bool aucuneRacine = true;

    for (int i = 0; i < n; i++) {
        if (!pourrait[i]) {
            continue;
        }

        bool racine = true;

        for (const string& nomAppel : methodes[i].appels) {
            int indice = trouverMethode(methodes, nomAppel);

            if (!methodes[indice].estConst) {
                racine = false;
                break;
            }
        }

        if (racine) {
            cout << " " << methodes[i].nom;
            aucuneRacine = false;
        }
    }

    if (aucuneRacine) {
        cout << " AUCUN";
    }
    cout << '\n';

    // Calculer les passes simultanément.
    vector<bool> estPret(n, false);

    for (int i = 0; i < n; i++) {
        estPret[i] = methodes[i].estConst;
    }

    int passes = 0;
    int restant = 0;

    for (int i = 0; i < n; i++) {
        if (pourrait[i]) {
            restant++;
        }
    }

    while (restant > 0) {
        vector<int> aDeclarer;

        for (int i = 0; i < n; i++) {
            if (!pourrait[i] || estPret[i]) {
                continue;
            }

            bool appelsPrets = true;

            for (const string& nomAppel : methodes[i].appels) {
                int indice = trouverMethode(methodes, nomAppel);

                if (!estPret[indice]) {
                    appelsPrets = false;
                    break;
                }
            }

            if (appelsPrets) {
                aDeclarer.push_back(i);
            }
        }

        for (int indice : aDeclarer) {
            estPret[indice] = true;
            restant--;
        }

        if (aDeclarer.empty()) {
            break;
        }

        passes++;
    }

    cout << "PASSES " << passes << '\n';

    return 0;
}
