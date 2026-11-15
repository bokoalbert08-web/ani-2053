#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

struct Image {
    string nom;
    bool selectionnee = false;
};

vector<string> nomsDansItem(const vector<Image>& planche) {
    vector<string> resultat;
    for (const auto& image : planche) {
        resultat.push_back(image.nom);
    }
    return resultat;
}

vector<string> selectionDansItem(const vector<Image>& planche) {
    vector<string> resultat;
    for (const auto& image : planche) {
        if (image.selectionnee) {
            resultat.push_back(image.nom);
        }
    }
    return resultat;
}

vector<string> selectionParIndex(
    const vector<string>& planche,
    const set<int>& selection
) {
    vector<string> resultat;
    for (size_t i = 0; i < planche.size(); ++i) {
        if (selection.count(static_cast<int>(i))) {
            resultat.push_back(planche[i]);
        }
    }
    return resultat;
}

vector<string> selectionParId(
    const vector<string>& planche,
    const set<string>& selection
) {
    vector<string> resultat;
    for (const auto& nom : planche) {
        if (selection.count(nom)) {
            resultat.push_back(nom);
        }
    }
    return resultat;
}

void afficher(const string& conception, const string& type,
              const vector<string>& elements) {
    cout << conception << ' ' << type;
    if (elements.empty()) {
        cout << " AUCUN";
    } else {
        for (const auto& element : elements) {
            cout << ' ' << element;
        }
    }
    cout << '\n';
}

int main() {
    vector<Image> dansItem;
    vector<string> plancheIndex;
    vector<string> plancheId;

    set<int> selectionIndex;
    set<string> selectionId;

    string commande;

    while (cin >> commande) {
        if (commande == "AJOUTE") {
            string nom;
            cin >> nom;

            dansItem.push_back({nom, false});
            plancheIndex.push_back(nom);
            plancheId.push_back(nom);
        }
        else if (commande == "CLIC") {
            int index;
            cin >> index;

            for (auto& image : dansItem) {
                image.selectionnee = false;
            }
            selectionIndex.clear();
            selectionId.clear();

            if (index >= 0) {
                size_t i = static_cast<size_t>(index);

                if (i < dansItem.size()) {
                    dansItem[i].selectionnee = true;
                }
                if (i < plancheIndex.size()) {
                    selectionIndex.insert(index);
                }
                if (i < plancheId.size()) {
                    selectionId.insert(plancheId[i]);
                }
            }
        }
        else if (commande == "AJOUTE_SEL") {
            int index;
            cin >> index;

            if (index >= 0) {
                size_t i = static_cast<size_t>(index);

                if (i < dansItem.size()) {
                    dansItem[i].selectionnee = true;
                }
                if (i < plancheIndex.size()) {
                    selectionIndex.insert(index);
                }
                if (i < plancheId.size()) {
                    selectionId.insert(plancheId[i]);
                }
            }
        }
        else if (commande == "ECHANGE") {
            int i, j;
            cin >> i >> j;

            if (i >= 0 && j >= 0) {
                size_t a = static_cast<size_t>(i);
                size_t b = static_cast<size_t>(j);

                if (a < dansItem.size() && b < dansItem.size()) {
                    swap(dansItem[a], dansItem[b]);
                }
                if (a < plancheIndex.size() && b < plancheIndex.size()) {
                    swap(plancheIndex[a], plancheIndex[b]);
                }
                if (a < plancheId.size() && b < plancheId.size()) {
                    swap(plancheId[a], plancheId[b]);
                }
            }
        }
        else if (commande == "RETIRE") {
            int index;
            cin >> index;

            if (index >= 0) {
                size_t i = static_cast<size_t>(index);

                if (i < dansItem.size()) {
                    dansItem.erase(dansItem.begin() + index);
                }
                if (i < plancheIndex.size()) {
                    plancheIndex.erase(plancheIndex.begin() + index);
                }
                if (i < plancheId.size()) {
                    plancheId.erase(plancheId.begin() + index);
                }
            }
        }
        else if (commande == "SUPPRIME") {
            vector<string> nouvellesIndex;
            for (size_t i = 0; i < plancheIndex.size(); ++i) {
                if (!selectionIndex.count(static_cast<int>(i))) {
                    nouvellesIndex.push_back(plancheIndex[i]);
                }
            }
            plancheIndex = nouvellesIndex;
            selectionIndex.clear();

            vector<string> nouvellesId;
            for (const auto& nom : plancheId) {
                if (!selectionId.count(nom)) {
                    nouvellesId.push_back(nom);
                }
            }
            plancheId = nouvellesId;
            selectionId.clear();

            dansItem.erase(
                remove_if(dansItem.begin(), dansItem.end(),
                    [](const Image& image) {
                        return image.selectionnee;
                    }),
                dansItem.end()
            );
        }
    }

    vector<string> itemPlanche = nomsDansItem(dansItem);
    vector<string> itemSelection = selectionDansItem(dansItem);

    vector<string> indexSelection =
        selectionParIndex(plancheIndex, selectionIndex);
    vector<string> idSelection =
        selectionParId(plancheId, selectionId);

    afficher("DANS_ITEM", "PLANCHE", itemPlanche);
    afficher("DANS_ITEM", "SELECTION", itemSelection);

    afficher("PAR_INDEX", "PLANCHE", plancheIndex);
    afficher("PAR_INDEX", "SELECTION", indexSelection);

    afficher("PAR_ID", "PLANCHE", plancheId);
    afficher("PAR_ID", "SELECTION", idSelection);

    vector<string> faux;
    if (plancheIndex != itemPlanche || indexSelection != itemSelection) {
        faux.push_back("PAR_INDEX");
    }
    if (plancheId != itemPlanche || idSelection != itemSelection) {
        faux.push_back("PAR_ID");
    }

    cout << "FAUX"; for (const auto& nom : faux) cout << " " << nom; if (faux.empty()) cout << " AUCUN"; cout << "\n";

    return 0;
}
