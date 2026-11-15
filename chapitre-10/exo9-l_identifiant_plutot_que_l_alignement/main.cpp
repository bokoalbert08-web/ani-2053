#include <iostream>
#include <vector>
#include <string>
#include <utility>

using namespace std;

int main() {
    int n;
    cin >> n;

    vector<int> ordre;

    int prochainId = 1;
    int occasions = 0;
    int repercutes = 0;
    int recherches = 0;

    for (int operation = 0; operation < n; operation++) {
        string commande;
        cin >> commande;

        if (commande == "AJOUTE") {
            ordre.push_back(prochainId);
            prochainId++;
        }
        else if (commande == "SUPPRIME") {
            int c;
            cin >> c;

            vector<int> indices(c);

            for (int i = 0; i < c; i++) {
                cin >> indices[i];
            }

            vector<int> nouvelOrdre;
            int positionSupprimee = 0;

            for (int i = 0; i < static_cast<int>(ordre.size()); i++) {
                if (positionSupprimee < c &&
                    i == indices[positionSupprimee]) {
                    positionSupprimee++;
                } else {
                    nouvelOrdre.push_back(ordre[i]);
                }
            }

            ordre = nouvelOrdre;
            occasions++;
            repercutes += 2 * c;
        }
        else if (commande == "MONTE") {
            int i;
            cin >> i;

            if (i + 1 < static_cast<int>(ordre.size())) {
                swap(ordre[i], ordre[i + 1]);
                occasions++;
                repercutes += 2;
            }
        }
        else if (commande == "DESCEND") {
            int i;
            cin >> i;

            if (i > 0) {
                swap(ordre[i], ordre[i - 1]);
                occasions++;
                repercutes += 2;
            }
        }
        else if (commande == "DESSINE") {
            recherches += static_cast<int>(ordre.size());
        }
    }

    cout << "ORDRE";

    if (ordre.empty()) {
        cout << " AUCUN";
    } else {
        for (int id : ordre) {
            cout << " " << id;
        }
    }

    cout << '\n';
    cout << "OCCASIONS " << occasions << '\n';
    cout << "REPERCUTES " << repercutes << '\n';
    cout << "RECHERCHES " << recherches << '\n';

    return 0;
}
