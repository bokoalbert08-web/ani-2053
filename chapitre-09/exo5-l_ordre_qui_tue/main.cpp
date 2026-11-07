#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct Objet {
    string nom;
    bool libere = false;
    vector<int> dependances;
};

int main() {
    vector<Objet> objets;
    vector<vector<int>> blocs(1);
    unordered_map<string, int> indexObjets;

    string ligne;
    bool plantage = false;

    auto liberer = [&](int index, const string& type) {
        Objet& objet = objets[index];

        if (objet.libere) {
            return true;
        }

        for (int dependance : objet.dependances) {
            if (objets[dependance].libere) {
                cout << "PLANTAGE " << objet.nom
                     << " " << objets[dependance].nom << '\n';
                plantage = true;
                return false;
            }
        }

        objet.libere = true;
        cout << type << " " << objet.nom << '\n';
        return true;
    };

    auto fermerBloc = [&](int bloc) {
        for (int i = static_cast<int>(blocs[bloc].size()) - 1;
             i >= 0; i--) {
            if (!liberer(blocs[bloc][i], "DETRUIT")) {
                return false;
            }
        }
        return true;
    };

    while (getline(cin, ligne) && !plantage) {
        stringstream flux(ligne);
        string commande;
        flux >> commande;

        if (commande == "DECL") {
            string nom;
            flux >> nom;

            int index = static_cast<int>(objets.size());
            objets.push_back({nom, false, {}});
            indexObjets[nom] = index;
            blocs.back().push_back(index);
        }
        else if (commande == "DEPEND") {
            string a, b;
            flux >> a >> b;

            objets[indexObjets[a]].dependances.push_back(indexObjets[b]);
        }
        else if (commande == "FERME") {
            string nom;
            flux >> nom;

            int index = indexObjets[nom];
            liberer(index, "FERME");
        }
        else if (commande == "{") {
            blocs.push_back({});
        }
        else if (commande == "}") {
            if (fermerBloc(static_cast<int>(blocs.size()) - 1)) {
                blocs.pop_back();
            }
        }
    }

    while (!plantage && !blocs.empty()) {
        if (!fermerBloc(static_cast<int>(blocs.size()) - 1)) {
            break;
        }
        blocs.pop_back();
    }

    if (!plantage) {
        cout << "PROPRE\n";
    }

    return 0;
}
