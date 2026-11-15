#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

void ajouter(vector<string>& tableau, const string& nom) {
    tableau.push_back(nom);
}

void supprimer(vector<string>& tableau, const vector<int>& indices) {
    for (int i = static_cast<int>(indices.size()) - 1; i >= 0; i--) {
        int index = indices[i];

        if (index >= 0 && index < static_cast<int>(tableau.size())) {
            tableau.erase(tableau.begin() + index);
        }
    }
}

void echanger(vector<string>& tableau, int index) {
    if (index >= 0 && index + 1 < static_cast<int>(tableau.size())) {
        swap(tableau[index], tableau[index + 1]);
    }
}

bool contient(const vector<string>& noms, const string& nom) {
    return find(noms.begin(), noms.end(), nom) != noms.end();
}

int main() {
    string ligneAjout, ligneSuppression, ligneEchange;

    getline(cin, ligneAjout);
    getline(cin, ligneSuppression);
    getline(cin, ligneEchange);

    vector<string> nomsAjout;
    vector<string> nomsSuppression;
    vector<string> nomsEchange;

    auto lireNoms = [](const string& ligne, vector<string>& noms) {
        string mot;
        size_t position = ligne.find(' ');

        if (position == string::npos) {
            return;
        }

        string reste = ligne.substr(position + 1);
        size_t debut = 0;

        while (debut < reste.size()) {
            size_t fin = reste.find(' ', debut);

            if (fin == string::npos) {
                fin = reste.size();
            }

            if (fin > debut) {
                noms.push_back(reste.substr(debut, fin - debut));
            }

            debut = fin + 1;
        }
    };

    lireNoms(ligneAjout, nomsAjout);
    lireNoms(ligneSuppression, nomsSuppression);
    lireNoms(ligneEchange, nomsEchange);

    vector<string> modele;
    vector<string> textures;
    vector<string> sources;

    int n;
    cin >> n;

    for (int operation = 0; operation < n; operation++) {
        string commande;
        cin >> commande;

        if (commande == "AJOUTE") {
            string nom;
            cin >> nom;

            ajouter(modele, nom);

            if (contient(nomsAjout, "textures")) {
                ajouter(textures, nom);
            }

            if (contient(nomsAjout, "sources")) {
                ajouter(sources, nom);
            }
        }
        else if (commande == "SUPPRIME") {
            int c;
            cin >> c;

            vector<int> indices(c);

            for (int i = 0; i < c; i++) {
                cin >> indices[i];
            }

            supprimer(modele, indices);

            if (contient(nomsSuppression, "textures")) {
                supprimer(textures, indices);
            }

            if (contient(nomsSuppression, "sources")) {
                supprimer(sources, indices);
            }
        }
        else if (commande == "MONTE") {
            int index;
            cin >> index;

            if (index + 1 < static_cast<int>(modele.size())) {
                echanger(modele, index);

                if (contient(nomsEchange, "textures")) {
                    echanger(textures, index);
                }

                if (contient(nomsEchange, "sources")) {
                    echanger(sources, index);
                }
            }
        }
        else if (commande == "DESCEND") {
            int index;
            cin >> index;

            if (index > 0) {
                echanger(modele, index - 1);

                if (contient(nomsEchange, "textures")) {
                    echanger(textures, index - 1);
                }

                if (contient(nomsEchange, "sources")) {
                    echanger(sources, index - 1);
                }
            }
        }
    }

    size_t longueur = max({modele.size(), textures.size(), sources.size()});
    int decales = 0;

    for (size_t i = 0; i < longueur; i++) {
        string m = i < modele.size() ? modele[i] : "-";
        string t = i < textures.size() ? textures[i] : "-";
        string s = i < sources.size() ? sources[i] : "-";

        cout << i << " " << m << " " << t << " " << s << "\n";

        if (m != t || m != s) {
            decales++;
        }
    }

    cout << "DECALES " << decales << "\n";

    return 0;
}
