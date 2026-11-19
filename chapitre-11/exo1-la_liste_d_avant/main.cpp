#include <iostream>
#include <string>
#include <vector>
#include <set>

using namespace std;

int main() {
    int n, c, a;
    string mot;

    cin >> mot >> n;

    vector<string> avant(n);
    for (int i = 0; i < n; ++i) {
        cin >> avant[i];
    }

    set<string> corrige;
    cin >> mot >> c;

    for (int i = 0; i < c; ++i) {
        string cle;
        cin >> cle;
        corrige.insert(cle);
    }

    set<string> apres;
    cin >> mot >> a;

    for (int i = 0; i < a; ++i) {
        string cle;
        cin >> cle;
        apres.insert(cle);
    }

    int vraies = 0;
    int fausses = 0;
    int oubliees = 0;
    int nouvelles = 0;

    for (int i = 0; i < n; ++i) {
        const string& cle = avant[i];

        if (corrige.count(cle)) {
            cout << i + 1 << ' ' << cle << " DEVENUE_FAUSSE\n";
            ++fausses;
        }
        else if (apres.count(cle)) {
            cout << i + 1 << ' ' << cle << " ENCORE_VRAIE\n";
            ++vraies;
        }
        else {
            cout << i + 1 << ' ' << cle << " OUBLIEE\n";
            ++oubliees;
        }
    }

    for (const string& cle : apres) {
        bool existe = false;

        for (const string& ancienne : avant) {
            if (cle == ancienne) {
                existe = true;
                break;
            }
        }

        if (!existe) {
            ++nouvelles;
        }
    }

    int partOubliee = 0;
    if (vraies + oubliees > 0) {
        partOubliee = 100 * oubliees / (vraies + oubliees);
    }

    cout << "VRAIES " << vraies << '\n';
    cout << "FAUSSES " << fausses << '\n';
    cout << "OUBLIEES " << oubliees << '\n';
    cout << "NOUVELLES " << nouvelles << '\n';
    cout << "PART_OUBLIEE " << partOubliee << '\n';

    return 0;
}
