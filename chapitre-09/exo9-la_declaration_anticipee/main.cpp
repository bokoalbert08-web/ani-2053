#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

int main() {
    int u;
    cin >> u;

    vector<string> types;
    vector<bool> besoinDefinition;
    unordered_map<string, int> indice;

    for (int i = 0; i < u; i++) {
        string type, emploi;
        cin >> type >> emploi;

        if (indice.find(type) == indice.end()) {
            indice[type] = types.size();
            types.push_back(type);
            besoinDefinition.push_back(false);
        }

        if (emploi == "VALEUR" || emploi == "BASE" ||
            emploi == "CORPS" || emploi == "TAILLE") {
            besoinDefinition[indice[type]] = true;
        }
    }

    int a;
    cin >> a;

    unordered_map<string, bool> inclusAujourdHui;

    for (int i = 0; i < a; i++) {
        string type;
        cin >> type;
        inclusAujourdHui[type] = true;
    }

    int nombreInclure = 0;
    int nombreAnticiper = 0;
    int inclusionsRetirees = 0;

    for (int i = 0; i < static_cast<int>(types.size()); i++) {
        if (besoinDefinition[i]) {
            cout << types[i] << " INCLURE\n";
            nombreInclure++;
        } else {
            cout << types[i] << " ANTICIPER\n";
            nombreAnticiper++;

            if (inclusAujourdHui[types[i]]) {
                inclusionsRetirees++;
            }
        }
    }

    cout << "INCLURE " << nombreInclure << '\n';
    cout << "ANTICIPER " << nombreAnticiper << '\n';
    cout << "INCLUSIONS_RETIREES " << inclusionsRetirees << '\n';

    return 0;
}
