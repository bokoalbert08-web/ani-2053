#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
using namespace std;

int main() {
    int n;
    cin >> n;

    vector<string> fichiers(n);
    vector<vector<string>> nomsInclus(n);
    unordered_map<string, int> indice;

    for (int i = 0; i < n; i++) {
        cin >> fichiers[i];
        indice[fichiers[i]] = i;

        int k;
        cin >> k;

        nomsInclus[i].resize(k);

        for (int j = 0; j < k; j++) {
            cin >> nomsInclus[i][j];
        }
    }

    vector<vector<int>> dependants(n);

    for (int i = 0; i < n; i++) {
        for (const string& nom : nomsInclus[i]) {
            auto it = indice.find(nom);

            if (it != indice.end()) {
                dependants[it->second].push_back(i);
            }
        }
    }

    auto estCpp = [](const string& nom) {
        return nom.size() >= 4 &&
               nom.substr(nom.size() - 4) == ".cpp";
    };

    auto estHeader = [](const string& nom) {
        return nom.size() >= 2 &&
               nom.substr(nom.size() - 2) == ".h";
    };

    auto compter = [&](int depart) {
        vector<bool> visite(n, false);
        int total = 0;

        function<void(int)> dfs = [&](int courant) {
            if (visite[courant]) return;

            visite[courant] = true;

            if (estCpp(fichiers[courant])) {
                total++;
            }

            for (int suivant : dependants[courant]) {
                dfs(suivant);
            }
        };

        dfs(depart);
        return total;
    };

    int q;
    cin >> q;

    for (int i = 0; i < q; i++) {
        string question;
        cin >> question;

        int total = compter(indice[question]);

        cout << question << " " << total << '\n';
    }

    string plusLourd = "AUCUN";
    int maximum = -1;

    for (int i = 0; i < n; i++) {
        if (estHeader(fichiers[i])) {
            int total = compter(i);

            if (total > maximum) {
                maximum = total;
                plusLourd = fichiers[i];
            }
        }
    }

    if (plusLourd == "AUCUN") {
        cout << "PLUS_LOURD AUCUN\n";
    } else {
        cout << "PLUS_LOURD " << plusLourd
             << " " << maximum << '\n';
    }

    return 0;
}
