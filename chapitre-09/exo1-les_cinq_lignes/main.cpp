#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <string>

enum class Etat {
    Absent,
    Ecrit,
    Defaut,
    Supprime
};

const std::array<std::string, 5> noms = {
    "DESTRUCTEUR",
    "COPIE",
    "AFFECTATION_COPIE",
    "DEPLACEMENT",
    "AFFECTATION_DEPLACEMENT"
};

std::string texteEtat(Etat etat) {
    switch (etat) {
        case Etat::Ecrit: return "ECRIT";
        case Etat::Defaut: return "DEFAUT";
        case Etat::Supprime: return "SUPPRIME";
        default: return "ABSENT";
    }
}

bool commencePar(const std::string& ligne, const std::string& prefixe) {
    return ligne.compare(0, prefixe.size(), prefixe) == 0;
}

bool terminePar(const std::string& ligne, const std::string& suffixe) {
    return ligne.size() >= suffixe.size()
        && ligne.compare(ligne.size() - suffixe.size(),
                         suffixe.size(), suffixe) == 0;
}

int main() {
    std::string classe;

    if (!std::getline(std::cin, classe)) {
        return 0;
    }

    std::array<Etat, 5> etats = {
        Etat::Absent,
        Etat::Absent,
        Etat::Absent,
        Etat::Absent,
        Etat::Absent
    };

    std::string ligne;

    while (std::getline(std::cin, ligne)) {
        std::size_t commentaire = ligne.find("//");

        if (commentaire != std::string::npos) {
            ligne.erase(commentaire);
        }

        ligne.erase(
            std::remove_if(ligne.begin(), ligne.end(),
                [](unsigned char caractere) {
                    return std::isspace(caractere);
                }),
            ligne.end()
        );

        if (ligne.empty()) {
            continue;
        }

        int membre = -1;

        if (ligne.find("~" + classe + "(") != std::string::npos) {
            membre = 0;
        }
        else if (commencePar(ligne, classe + "(const" + classe + "&")
                 && ligne.size() > (classe + "(const" + classe + "&").size()
                 && ligne[(classe + "(const" + classe + "&").size()] != '&') {
            membre = 1;
        }
        else if (commencePar(ligne, classe + "&operator=(const" + classe + "&")
                 && ligne.size() > (classe + "&operator=(const" + classe + "&").size()
                 && ligne[(classe + "&operator=(const" + classe + "&").size()] != '&') {
            membre = 2;
        }
        else if (commencePar(ligne, classe + "(" + classe + "&&")) {
            membre = 3;
        }
        else if (commencePar(ligne, classe + "&operator=(" + classe + "&&")) {
            membre = 4;
        }

        if (membre == -1 || etats[membre] != Etat::Absent) {
            continue;
        }

        if (terminePar(ligne, "=delete;")) {
            etats[membre] = Etat::Supprime;
        }
        else if (terminePar(ligne, "=default;")) {
            etats[membre] = Etat::Defaut;
        }
        else {
            etats[membre] = Etat::Ecrit;
        }
    }

    for (std::size_t i = 0; i < etats.size(); ++i) {
        std::cout << noms[i] << " " << texteEtat(etats[i]) << '\n';
    }

    int declares = 0;

    for (Etat etat : etats) {
        if (etat != Etat::Absent) {
            ++declares;
        }
    }

    bool deplacement =
        etats[3] != Etat::Absent || etats[4] != Etat::Absent;

    std::string regle;

    if (declares == 0) {
        regle = "ZERO";
    }
    else if (declares == 5) {
        regle = "CINQ";
    }
    else if (etats[0] != Etat::Absent
             && etats[1] != Etat::Absent
             && etats[2] != Etat::Absent
             && etats[3] == Etat::Absent
             && etats[4] == Etat::Absent) {
        regle = "TROIS";
    }
    else {
        regle = "INCOMPLETE";
    }

    std::cout << "REGLE " << regle << '\n';

    if (regle == "INCOMPLETE") {
        int limite = deplacement ? 5 : 3;

        for (int i = 0; i < limite; ++i) {
            if (etats[i] == Etat::Absent) {
                std::cout << "MANQUE " << noms[i] << '\n';
            }
        }
    }

    bool possede = etats[0] == Etat::Ecrit;

    bool unique =
        etats[1] == Etat::Supprime
        && (etats[3] == Etat::Ecrit || etats[3] == Etat::Defaut);

    std::cout << "POSSEDE " << (possede ? "OUI" : "NON") << '\n';
    std::cout << "UNIQUE " << (unique ? "OUI" : "NON") << '\n';

    return 0;
}
