#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main() {
    vector<string> pile;
    vector<int> traits;
    string operation;

    int numeroTrait = 0;
    int numeroAnnulation = 0;
    int justes = 0;
    int riens = 0;
    int trahis = 0;
    int operations = 0;
    int annulables = 0;

    while (cin >> operation) {
        if (operation == "ANNULE") {
            ++numeroAnnulation;

            string attendu = "AUCUNE";
            if (!pile.empty()) {
                attendu = pile.back();
                pile.pop_back();
            }

            string obtenu = "RIEN";
            if (!traits.empty()) {
                obtenu = "TRAIT " + to_string(traits.back());
                traits.pop_back();
            }

            string verdict;

            if (attendu == "AUCUNE" && obtenu == "RIEN") {
                verdict = "JUSTE";
                ++justes;
            }
            else if (obtenu == "RIEN") {
                verdict = "RIEN";
                ++riens;
            }
            else if (attendu == obtenu) {
                verdict = "JUSTE";
                ++justes;
            }
            else {
                verdict = "TRAHI";
                ++trahis;
            }

            cout << "ANNULE " << numeroAnnulation
                 << " ATTENDU " << attendu
                 << " OBTENU " << obtenu
                 << ' ' << verdict << '\n';
        }
        else {
            ++operations;

            if (operation == "TRAIT") {
                ++numeroTrait;
                ++annulables;
                pile.push_back("TRAIT " + to_string(numeroTrait));
                traits.push_back(numeroTrait);
            }
            else if (operation == "EFFACE") {
                pile.push_back("EFFACE");
                traits.clear();
            }
            else if (operation == "RANGE") {
                pile.push_back("RANGE");
            }
            else {
                string nom;
                cin >> nom;
                pile.push_back(operation + " " + nom);
            }
        }
    }

    cout << "JUSTES " << justes << '\n';
    cout << "RIENS " << riens << '\n';
    cout << "TRAHIS " << trahis << '\n';
    cout << "ANNULABLES " << annulables
         << " SUR " << operations << '\n';

    return 0;
}
