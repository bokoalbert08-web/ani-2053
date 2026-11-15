#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

struct Image {
    string nom;
    double x, y;
    double largeur, hauteur;
    double echelle;
};

bool touche(const Image& image, double px, double py) {
    double demiLargeur = image.largeur * image.echelle / 2.0;
    double demiHauteur = image.hauteur * image.echelle / 2.0;

    return abs(px - image.x) <= demiLargeur &&
           abs(py - image.y) <= demiHauteur;
}

int main() {
    int n;
    cin >> n;

    vector<Image> images(n);

    for (int i = 0; i < n; i++) {
        cin >> images[i].nom
            >> images[i].x
            >> images[i].y
            >> images[i].largeur
            >> images[i].hauteur
            >> images[i].echelle;
    }

    int q;
    cin >> q;

    int erreurs = 0;

    for (int k = 1; k <= q; k++) {
        double px, py;
        cin >> px >> py;

        string arriere = "AUCUN";
        string endroit = "AUCUN";

        for (int i = n - 1; i >= 0; i--) {
            if (touche(images[i], px, py)) {
                arriere = images[i].nom;
                break;
            }
        }

        for (int i = 0; i < n; i++) {
            if (touche(images[i], px, py)) {
                endroit = images[i].nom;
                break;
            }
        }

        if (arriere != endroit) {
            erreurs++;
        }

        cout << "CLIC " << k << " "
             << arriere << " " << endroit << "\n";
    }

    cout << "ERREURS " << erreurs << "\n";

    return 0;
}
