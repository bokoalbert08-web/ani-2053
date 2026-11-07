#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct Texture {
    int numero;
    bool liberee;
};

bool liberer(Texture& texture) {
    if (texture.liberee) {
        return false;
    }

    texture.liberee = true;
    return true;
}

int main() {
    int S, V, F;
    cin >> S >> V >> F;

    int N;
    cin >> N;

    vector<Texture> textures;
    vector<int> tableau;

    int nombreCreees = 0;

    for (int i = 1; i <= N; i++) {
        string operation;
        cin >> operation;

        if (operation == "AJOUTE") {
            nombreCreees++;

            textures.push_back({nombreCreees, false});
            tableau.push_back(nombreCreees);
        }

        else if (operation == "SUPPRIME") {
            int index;
            cin >> index;

            int numero = tableau[index];

            for (int j = 0; j < S; j++) {
                if (!liberer(textures[numero - 1])) {
                    cout << "PLANTAGE " << i
                         << " TEXTURE " << numero << '\n';
                    return 0;
                }
            }

            tableau.erase(tableau.begin() + index);
        }

        else if (operation == "VIDE") {
            for (int numero : tableau) {
                for (int j = 0; j < V; j++) {
                    if (!liberer(textures[numero - 1])) {
                        cout << "PLANTAGE " << i
                             << " TEXTURE " << numero << '\n';
                        return 0;
                    }
                }
            }

            tableau.clear();
        }
    }

    for (int numero : tableau) {
        for (int j = 0; j < F; j++) {
            if (!liberer(textures[numero - 1])) {
                cout << "PLANTAGE FIN TEXTURE "
                     << numero << '\n';
                return 0;
            }
        }
    }

    int fuites = 0;

    for (const Texture& texture : textures) {
        if (!texture.liberee) {
            fuites++;
        }
    }

    cout << "CREEES " << nombreCreees << '\n';
    cout << "FUITES " << fuites << '\n';
    cout << "PERDUES";

    bool aucunePerdue = true;

    for (const Texture& texture : textures) {
        if (!texture.liberee) {
            cout << " " << texture.numero;
            aucunePerdue = false;
        }
    }

    if (aucunePerdue) {
        cout << " AUCUN";
    }

    cout << '\n';

    return 0;
}
