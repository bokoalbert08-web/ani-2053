// La course, provoquée : huit fils incrémentent le même compteur,
// un million de fois chacun, sans aucune protection.

#include <NKThreading/NkThread.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace nkentseu;
using namespace nkentseu::threading;

static const long long kIncrements = 1000000;

// Le compteur partagé : un simple entier, sans verrou ni atomique.
static long long compteur = 0;

int main(int argc, char** argv)
{
    // Nombre de fils : 8 par défaut, ou celui donné en argument.
    int nbFils = 8;
    if (argc > 1)
    {
        nbFils = std::atoi(argv[1]);
        if (nbFils < 1)
        {
            nbFils = 1;
        }
    }

    std::vector<NkThread> fils;
    fils.reserve(nbFils);
    for (int i = 0; i < nbFils; ++i)
    {
        fils.emplace_back([](void*)
        {
            for (long long k = 0; k < kIncrements; ++k)
            {
                compteur += 1;
            }
        });
    }

    // On lit le total seulement après le Join de chaque fil.
    for (int i = 0; i < nbFils; ++i)
    {
        fils[i].Join();
    }

    long long attendu = (long long)nbFils * kIncrements;
    std::printf("fils : %d | total : %lld | attendu : %lld | perdus : %lld\n",
                nbFils, compteur, attendu, attendu - compteur);
    return 0;
}
