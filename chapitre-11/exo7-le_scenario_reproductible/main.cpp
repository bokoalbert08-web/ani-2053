#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct Action {
    long long tour;
    string type;
    string a;
    string b;
    string c;
};

bool lireEntier(const string& texte, long long& nombre) {
    try {
        size_t position = 0;
        nombre = stoll(texte, &position);
        return position == texte.size();
    } catch (...) {
        return false;
    }
}

vector<string> decouper(const string& texte, char separateur) {
    vector<string> morceaux;
    string morceau;
    stringstream flux(texte);

    while (getline(flux, morceau, separateur)) {
        morceaux.push_back(morceau);
    }

    if (!texte.empty() && texte.back() == separateur) {
        morceaux.push_back("");
    }

    if (texte.empty()) {
        morceaux.push_back("");
    }

    return morceaux;
}

int main() {
    vector<Action> pan;
    vector<Action> zoom;
    vector<Action> depot;
    vector<Action> clic;
    vector<Action> move;
    vector<Action> pack;
    vector<Action> grille;
    vector<long long> captures;

    long long sortie = 0;
    bool sortiePresente = false;
    long long ignores = 0;

    string ligne;

    while (getline(cin, ligne)) {
        size_t egal = ligne.find('=');

        if (egal == string::npos) {
            ignores++;
            continue;
        }

        string nom = ligne.substr(0, egal);
        string valeur = ligne.substr(egal + 1);

        if (nom == "NK_AGENT_SHOT") {
            vector<string> elements = decouper(valeur, ',');

            for (const string& element : elements) {
                long long n;

                if (!lireEntier(element, n)) {
                    ignores++;
                } else if (captures.size() < 8) {
                    captures.push_back(n);
                } else {
                    ignores++;
                }
            }
        }
        else if (nom == "NK_AGENT_EXIT") {
            if (!lireEntier(valeur, sortie)) {
                ignores++;
            } else {
                sortiePresente = true;
            }
        }
        else if (nom == "NK_AGENT_PAN" ||
                 nom == "NK_AGENT_MOVE" ||
                 nom == "NK_AGENT_ZOOM" ||
                 nom == "NK_AGENT_CLICK" ||
                 nom == "NK_AGENT_GRID") {

            vector<string> champs = decouper(valeur, ':');
            long long n;

            size_t minimum = 3;
            if (nom == "NK_AGENT_ZOOM") minimum = 4;
            if (nom == "NK_AGENT_GRID") minimum = 2;

            if (champs.size() < minimum || !lireEntier(champs[0], n)) {
                ignores++;
                continue;
            }

            if (nom == "NK_AGENT_PAN") {
                pan.push_back({n, "PAN", champs[1], champs[2], ""});
            }
            else if (nom == "NK_AGENT_MOVE") {
                move.push_back({n, "MOVE", champs[1], champs[2], ""});
            }
            else if (nom == "NK_AGENT_ZOOM") {
                zoom.push_back({n, "ZOOM", champs[1], champs[2], champs[3]});
            }
            else if (nom == "NK_AGENT_GRID") {
                grille.push_back({n, "GRILLE", champs[1], "", ""});
            }
            else {
                vector<string> elements = decouper(champs.size() >= 3
                    ? valeur.substr(valeur.find(':') + 1)
                    : "", ';');

                // Le clic est traité comme une liste de positions.
                for (const string& element : elements) {
                    vector<string> position = decouper(element, ':');
                    long long tour;

                    if (position.size() < 2 ||
                        !lireEntier(champs[0], tour)) {
                        ignores++;
                        continue;
                    }

                    if (clic.size() < 8) {
                        clic.push_back({tour, "CLIC", position[0],
                                        position[1], ""});
                    } else {
                        ignores++;
                    }
                }
            }
        }
        else if (nom == "NK_AGENT_DROP") {
            size_t p1 = valeur.find(':');
            size_t p2 = p1 == string::npos ? string::npos : valeur.find(':', p1 + 1);
            size_t p3 = p2 == string::npos ? string::npos : valeur.find(':', p2 + 1);

            long long n;

            if (p3 == string::npos ||
                !lireEntier(valeur.substr(0, p1), n)) {
                ignores++;
                continue;
            }

            string px = valeur.substr(p1 + 1, p2 - p1 - 1);
            string py = valeur.substr(p2 + 1, p3 - p2 - 1);
            string chemins = valeur.substr(p3 + 1);

            vector<string> elements = decouper(chemins, ';');

            for (const string& chemin : elements) {
                if (depot.size() < 8) {
                    depot.push_back({n, "DEPOT", chemin, px, py});
                } else {
                    ignores++;
                }
            }
        }
        else if (nom == "NK_AGENT_PACK") {
            long long n;

            if (!lireEntier(valeur, n)) {
                ignores++;
            } else {
                pack.push_back({n, "RANGE", "", "", ""});
            }
        }
        else {
            ignores++;
        }
    }

    auto afficher = [](const vector<Action>& actions, long long tour) {
        for (const Action& action : actions) {
            if (action.tour != tour || action.tour <= 0) {
                continue;
            }

            cout << "TOUR " << tour << " " << action.type;

            if (action.type == "PAN" || action.type == "MOVE") {
                cout << " " << action.a << " " << action.b;
            }
            else if (action.type == "ZOOM") {
                cout << " " << action.a << " " << action.b
                     << " " << action.c;
            }
            else if (action.type == "DEPOT") {
                cout << " " << action.a << " " << action.b
                     << " " << action.c;
            }
            else if (action.type == "CLIC") {
                cout << " " << action.a << " " << action.b;
            }
            else if (action.type == "GRILLE") {
                cout << " " << action.a;
            }

            cout << '\n';
        }
    };

    long long dernierTour = -1;

    auto mettreAJourDernierTour = [&](const vector<Action>& actions) {
        for (const Action& action : actions) {
            if (action.tour > dernierTour) {
                dernierTour = action.tour;
            }
        }
    };

    mettreAJourDernierTour(pan);
    mettreAJourDernierTour(zoom);
    mettreAJourDernierTour(depot);
    mettreAJourDernierTour(clic);
    mettreAJourDernierTour(move);
    mettreAJourDernierTour(pack);
    mettreAJourDernierTour(grille);

    for (long long n : captures) {
        if (n - 1 > dernierTour) {
            dernierTour = n - 1;
        }
    }

    bool arret = sortiePresente && sortie > 0;

    if (arret) {
        dernierTour = sortie - 1;
    }

    for (long long tour = 0; tour <= dernierTour; tour++) {
        afficher(pan, tour);
        afficher(zoom, tour);
        afficher(depot, tour);
        afficher(clic, tour);
        afficher(move, tour);
        afficher(pack, tour);
        afficher(grille, tour);

        for (long long n : captures) {
            if (n == tour + 1 && n > 0) {
                cout << "TOUR " << n << " CAPTURE\n";
            }
        }
    }

    if (arret) {
        cout << "ARRET " << sortie - 1 << '\n';
    } else {
        cout << "SANS_ARRET\n";
    }

    long long jamais = 0;

    auto compterJamais = [&](const vector<Action>& actions) {
        for (const Action& action : actions) {
            if (action.tour <= 0 ||
                (arret && action.tour >= sortie)) {
                jamais++;
            }
        }
    };

    compterJamais(pan);
    compterJamais(zoom);
    compterJamais(depot);
    compterJamais(clic);
    compterJamais(move);
    compterJamais(pack);
    compterJamais(grille);

    for (long long n : captures) {
        if (n <= 0 || (arret && n > sortie)) {
            jamais++;
        }
    }

    cout << "JAMAIS " << jamais << '\n';
    cout << "IGNORES " << ignores << '\n';

    return 0;
}
