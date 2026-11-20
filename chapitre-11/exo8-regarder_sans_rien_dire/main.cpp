#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Contournement {
    string nom;
    int fois = 0;
    int premiereObservation = 0;
    int ordre = 0;
    bool dit = false;
};

int convertirTemps(const string& temps) {
    int minutes = stoi(temps.substr(0, 2));
    int secondes = stoi(temps.substr(3, 2));
    return minutes * 60 + secondes;
}

int main() {
    string commande;
    int n;
    cin >> commande >> n;

    vector<Contournement> observations;
    int horsSeance = 0;
    int totalObservations = 0;

    for (int i = 0; i < n; i++) {
        string temps, nom;
        cin >> temps >> nom;

        int instant = convertirTemps(temps);

        if (instant > 20 * 60) {
            horsSeance++;
            continue;
        }

        totalObservations++;

        auto it = find_if(observations.begin(), observations.end(),
            [&](const Contournement& c) {
                return c.nom == nom;
            });

        if (it == observations.end()) {
            observations.push_back({nom, 1, instant, i, false});
        } else {
            it->fois++;
            it->premiereObservation = min(it->premiereObservation, instant);
            it->ordre = min(it->ordre, i);
        }
    }

    int m;
    cin >> commande >> m;

    vector<string> cites;
    for (int i = 0; i < m; i++) {
        string nom;
        cin >> nom;
        cites.push_back(nom);
    }

    for (auto& observation : observations) {
        observation.dit = find(cites.begin(), cites.end(),
                               observation.nom) != cites.end();
    }

    sort(observations.begin(), observations.end(),
        [](const Contournement& a, const Contournement& b) {
            if (a.fois != b.fois) {
                return a.fois > b.fois;
            }
            if (a.premiereObservation != b.premiereObservation) {
                return a.premiereObservation < b.premiereObservation;
            }
            return a.ordre < b.ordre;
        });

    int nonDits = 0;
    int foisNonDites = 0;

    for (const auto& observation : observations) {
        cout << observation.nom << " " << observation.fois << " "
             << (observation.dit ? "DIT" : "NON_DIT") << '\n';

        if (!observation.dit) {
            nonDits++;
            foisNonDites += observation.fois;
        }
    }

    int partNonDite = 0;
    if (totalObservations > 0) {
        partNonDite = 100 * foisNonDites / totalObservations;
    }

    cout << "NON_DITS " << nonDits << '\n';
    cout << "PART_NON_DITE " << partNonDite << '\n';

    vector<string> ditsSansVu;
    for (const string& nom : cites) {
        auto it = find_if(observations.begin(), observations.end(),
            [&](const Contournement& c) {
                return c.nom == nom;
            });

        if (it == observations.end()) {
            ditsSansVu.push_back(nom);
        }
    }

    cout << "DIT_SANS_VU";
    if (ditsSansVu.empty()) {
        cout << " AUCUN";
    } else {
        for (const string& nom : ditsSansVu) {
            cout << " " << nom;
        }
    }
    cout << '\n';

    cout << "HORS_SEANCE " << horsSeance << '\n';

    return 0;
}
