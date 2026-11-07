#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct Instruction {
    string commande;
    string a;
    string b;
    int numero;
};

int main() {
    int N;
    cin >> N;
    cin.ignore();

    vector<Instruction> instructions;
    vector<string> variables;
    unordered_map<string, bool> dejaVue;
    vector<int> copies;

    for (int i = 1; i <= N; i++) {
        string ligne;
        getline(cin, ligne);

        stringstream flux(ligne);
        Instruction instruction;
        instruction.numero = i;

        flux >> instruction.commande;

        if (instruction.commande == "NEW"
            || instruction.commande == "RESET"
            || instruction.commande == "USE") {
            flux >> instruction.a;
        }
        else if (instruction.commande == "MOVE"
                 || instruction.commande == "COPY") {
            flux >> instruction.a >> instruction.b;
        }

        if (!instruction.a.empty() && !dejaVue[instruction.a]) {
            dejaVue[instruction.a] = true;
            variables.push_back(instruction.a);
        }

        if (!instruction.b.empty() && !dejaVue[instruction.b]) {
            dejaVue[instruction.b] = true;
            variables.push_back(instruction.b);
        }

        if (instruction.commande == "COPY") {
            copies.push_back(i);
        }

        instructions.push_back(instruction);
    }

    if (!copies.empty()) {
        for (int numero : copies) {
            cout << "REFUSE " << numero << '\n';
        }
        return 0;
    }

    unordered_map<string, int> possession;

    int nombreCrees = 0;
    int nombreDetruits = 0;
    int nombreNuls = 0;

    auto detruire = [&](int objet) {
        if (objet != 0) {
            cout << "DETRUIT " << objet << '\n';
            nombreDetruits++;
        }
    };

    for (const Instruction& instruction : instructions) {
        const string& commande = instruction.commande;
        const string& a = instruction.a;
        const string& b = instruction.b;

        if (commande == "NEW") {
            int ancien = possession[a];

            nombreCrees++;
            possession[a] = nombreCrees;

            detruire(ancien);
        }
        else if (commande == "MOVE") {
            int objet = possession[a];

            possession[a] = 0;

            detruire(possession[b]);

            possession[b] = objet;
        }
        else if (commande == "RESET") {
            detruire(possession[a]);
            possession[a] = 0;
        }
        else if (commande == "USE") {
            if (possession[a] == 0) {
                cout << "NUL " << instruction.numero
                     << " " << a << '\n';
                nombreNuls++;
            }
        }
    }

    for (int i = static_cast<int>(variables.size()) - 1;
         i >= 0; i--) {
        detruire(possession[variables[i]]);
    }

    cout << "CREES " << nombreCrees << '\n';
    cout << "DETRUITS " << nombreDetruits << '\n';
    cout << "NULS " << nombreNuls << '\n';

    return 0;
}
