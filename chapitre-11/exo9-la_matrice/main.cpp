#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Entree {
    string nom;
    int valeur;
    int cout;
    string etiquette;
    string genre;
    string cible;
    int ordre;
    string raison;
    string caseMatrice;
};

int rangEtiquette(const string& etiquette) {
    if (etiquette == "MESUREE") return 0;
    if (etiquette == "OBSERVEE") return 1;
    return 2;
}

int rangCase(const string& caseMatrice) {
    if (caseMatrice == "MAINTENANT") return 0;
    if (caseMatrice == "DECOUPER") return 1;
    if (caseMatrice == "EN_PASSANT") return 2;
    return 3;
}

int main() {
    int n;
    cin >> n;

    vector<Entree> entrees;
    vector<Entree> gardees;
    vector<Entree> horsListe;

    for (int i = 0; i < n; i++) {
        Entree entree;
        entree.ordre = i;
        cin >> entree.nom >> entree.valeur >> entree.cout
            >> entree.etiquette >> entree.genre;

        if (entree.genre == "SOCLE") {
            cin >> entree.cible;
        }

        entree.raison = "";

        if (entree.genre == "REECRITURE") {
            entree.raison = "reecriture";
        } else if (entree.genre == "CHOIX") {
            entree.raison = "choix";
        } else if (entree.etiquette == "-") {
            entree.raison = "sans_etiquette";
        }

        entrees.push_back(entree);
    }

    for (const auto& entree : entrees) {
        if (entree.raison != "") {
            horsListe.push_back(entree);
        } else {
            gardees.push_back(entree);
        }
    }

    for (auto& entree : gardees) {
        if (entree.genre == "SOCLE") {
            auto cible = find_if(gardees.begin(), gardees.end(),
                [&](const Entree& autre) {
                    return autre.nom == entree.cible
                        && autre.genre == "MANQUE";
                });

            if (cible != gardees.end()) {
                entree.valeur = max(entree.valeur, cible->valeur);
            }
        }

        bool valeurForte = entree.valeur >= 3;
        bool coutEleve = entree.cout >= 3;

        if (valeurForte && !coutEleve) {
            entree.caseMatrice = "MAINTENANT";
        } else if (valeurForte && coutEleve) {
            entree.caseMatrice = "DECOUPER";
        } else if (!valeurForte && !coutEleve) {
            entree.caseMatrice = "EN_PASSANT";
        } else {
            entree.caseMatrice = "ON_NE_LE_FAIT_PAS";
        }
    }

    sort(gardees.begin(), gardees.end(),
        [](const Entree& a, const Entree& b) {
            if (rangCase(a.caseMatrice) != rangCase(b.caseMatrice)) {
                return rangCase(a.caseMatrice) < rangCase(b.caseMatrice);
            }

            if (rangEtiquette(a.etiquette) != rangEtiquette(b.etiquette)) {
                return rangEtiquette(a.etiquette) < rangEtiquette(b.etiquette);
            }

            if (a.valeur != b.valeur) {
                return a.valeur > b.valeur;
            }

            if (a.cout != b.cout) {
                return a.cout < b.cout;
            }

            return a.ordre < b.ordre;
        });

    for (const auto& entree : gardees) {
        cout << entree.caseMatrice << " " << entree.nom
             << " " << entree.etiquette << '\n';
    }

    for (const auto& entree : horsListe) {
        cout << "HORS_LISTE " << entree.nom
             << " " << entree.raison << '\n';
    }

    bool honnete = any_of(gardees.begin(), gardees.end(),
        [](const Entree& entree) {
            return entree.caseMatrice == "ON_NE_LE_FAIT_PAS";
        });

    cout << "VERDICT " << (honnete ? "HONNETE" : "COMPLAISANT") << '\n';

    return 0;
}
