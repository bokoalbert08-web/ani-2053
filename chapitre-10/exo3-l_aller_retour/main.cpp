#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <sstream>

std::string formatDeux(double valeur) {
    std::ostringstream sortie;
    sortie << std::fixed << std::setprecision(2) << valeur;

    std::string resultat = sortie.str();

    if (resultat == "-0.00") {
        resultat = "0.00";
    }

    return resultat;
}

int main() {
    double largeur, hauteur;
    std::cin >> largeur >> hauteur;

    double cx = 0.0;
    double cy = 0.0;
    double zoom = 1.0;

    std::string commande;

    while (std::cin >> commande) {
        double x, y;

        if (commande == "VERS_MONDE") {
            double px, py;
            std::cin >> px >> py;

            x = cx + (px - largeur / 2.0) / zoom;
            y = cy + (py - hauteur / 2.0) / zoom;

            std::cout << "MONDE "
                      << formatDeux(x) << " "
                      << formatDeux(y) << '\n';
        }
        else if (commande == "VERS_PIXEL") {
            double mx, my;
            std::cin >> mx >> my;

            x = (mx - cx) * zoom + largeur / 2.0;
            y = (my - cy) * zoom + hauteur / 2.0;

            std::cout << "PIXEL "
                      << formatDeux(x) << " "
                      << formatDeux(y) << '\n';
        }
        else if (commande == "DEPLACE") {
            double dx, dy;
            std::cin >> dx >> dy;

            cx -= dx / zoom;
            cy -= dy / zoom;
        }
        else if (commande == "ZOOM") {
            double facteur, px, py;
            std::cin >> facteur >> px >> py;

            double ax = cx + (px - largeur / 2.0) / zoom;
            double ay = cy + (py - hauteur / 2.0) / zoom;

            zoom = std::clamp(zoom * facteur, 0.02, 64.0);

            cx = ax - (px - largeur / 2.0) / zoom;
            cy = ay - (py - hauteur / 2.0) / zoom;
        }
        else if (commande == "GRILLE") {
            double minimum;
            std::cin >> minimum;

            double pas = 1.0;

            while (pas * zoom < minimum) {
                pas *= 2.0;
            }

            while (pas * 0.5 * zoom >= minimum) {
                pas *= 0.5;
            }

            std::cout << "GRILLE "
                      << std::fixed << std::setprecision(6)
                      << pas << '\n';
        }
        else if (commande == "ORIGINE") {
            cx = 0.0;
            cy = 0.0;
            zoom = 1.0;
        }
    }

    std::cout << "CENTRE "
              << formatDeux(cx) << " "
              << formatDeux(cy) << '\n';

    std::cout << "ZOOM " << formatDeux(zoom) << '\n';

    return 0;
}
