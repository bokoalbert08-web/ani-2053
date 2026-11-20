#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

int main() {
    string ligne;

    bool enCours = false;
    string nom;
    long long minimum = 0;
    long long parjour = 0;
    long long debut = 0;
    long long gestes = 0;
    long long attente = 0;
    long long totalJour = 0;

    while (getline(cin, ligne)) {
        istringstream lecture(ligne);

        long long instant;
        string action;

        lecture >> instant >> action;

        if (action == "DEBUT") {
            lecture >> nom >> minimum >> parjour;

            debut = instant;
            gestes = 0;
            attente = 0;
            enCours = true;
        } else if (enCours && action == "FIN") {
            string nomFin;
            lecture >> nomFin;

            long long duree = instant - debut;
            long long enTrop = gestes > minimum ? gestes - minimum : 0;
            long long jourSecondes = (attente * parjour) / 1000;
            totalJour += jourSecondes;

            cout << nom << " GESTES " << gestes
                 << " EN_TROP " << enTrop
                 << " DUREE " << duree
                 << " ATTENTE " << attente
                 << " JOUR " << jourSecondes / 60 << ":"
                 << setw(2) << setfill('0') << jourSecondes % 60
                 << setfill(' ') << "\n";

            enCours = false;
        } else if (enCours && (action == "CLIC" || action == "TOUCHE" || action == "MOLETTE" || action == "GLISSE")) {
            gestes++;
        } else if (enCours && action == "ATTENTE") {
            long long dureeAttente;
            lecture >> dureeAttente;
            attente += dureeAttente;
        }
    }

    if (enCours) {
        cout << nom << " INACHEVEE\n";
    }

    cout << "TOTAL_JOUR " << totalJour / 60 << ":"
         << setw(2) << setfill('0') << totalJour % 60 << "\n";

    return 0;
}
