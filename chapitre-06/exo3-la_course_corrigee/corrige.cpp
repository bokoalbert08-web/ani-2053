// La course, corrigée : quatre façons de compter jusqu'à huit millions
// avec huit fils : sans protection, mutex, atomique, et compteurs locaux.

#include <NKThreading/NkThread.h>
#include <NKThreading/NkMutex.h>
#include <NKThreading/NkScopedLock.h>
#include <NKCore/NkAtomic.h>
#include <NKTime/NkChrono.h>

#include <cstdio>
#include <vector>

using namespace nkentseu;
using namespace nkentseu::threading;

static const int kFils = 8;
static const long long kIncrements = 1000000;
static const int kMesures = 3;
static const long long kAttendu = (long long)kFils * kIncrements;

static long long gNu = 0;          // version sans protection
static long long gMutex = 0;       // version mutex
static NkMutex gVerrou;
static NkAtomicInt gAtomique;      // version atomique
static long long gLocaux[kFils];   // version locale : une case par fil

// Crée kFils fils qui exécutent travail(indice), les joint tous, et rend
// le temps écoulé en millisecondes. La création des fils est comptée dans
// le temps, comme dans les quatre versions : elle est la même pour toutes.
template <typename F>
static double Chronometrer(F travail)
{
    NkChrono chrono;
    std::vector<NkThread> fils;
    fils.reserve(kFils);
    for (int i = 0; i < kFils; ++i)
    {
        fils.emplace_back([travail, i](void*) { travail(i); });
    }
    for (int i = 0; i < kFils; ++i)
    {
        fils[i].Join();
    }
    return chrono.Elapsed().milliseconds;
}

// Sans protection. La barrière vide (asm volatile avec "memory") interdit
// au compilateur de fusionner le million d'additions en une seule : sans
// elle, la mesure ne dirait plus rien en Release.
static double MesurerNu(long long& total)
{
    gNu = 0;
    double ms = Chronometrer([](int)
    {
        for (long long k = 0; k < kIncrements; ++k)
        {
            gNu += 1;
            asm volatile("" ::: "memory");
        }
    });
    total = gNu;
    return ms;
}

// Mutex : le verrou de portée est pris autour de l'incrément seul,
// et non autour de toute la boucle.
static double MesurerMutex(long long& total)
{
    gMutex = 0;
    double ms = Chronometrer([](int)
    {
        for (long long k = 0; k < kIncrements; ++k)
        {
            threading::NkScopedLock<NkMutex> verrou(gVerrou);
            gMutex += 1;
        }
    });
    total = gMutex;
    return ms;
}

// Atomique : un FetchAdd(1) par incrément.
static double MesurerAtomique(long long& total)
{
    gAtomique.Store(0);
    double ms = Chronometrer([](int)
    {
        for (long long k = 0; k < kIncrements; ++k)
        {
            gAtomique.FetchAdd(1);
        }
    });
    total = (long long)gAtomique.Load();
    return ms;
}

// Local : chaque fil compte dans sa propre variable et la range une seule
// fois à la fin. Même barrière vide, sur la variable locale cette fois.
static double MesurerLocal(long long& total)
{
    for (int i = 0; i < kFils; ++i)
    {
        gLocaux[i] = 0;
    }
    double ms = Chronometrer([](int i)
    {
        long long local = 0;
        for (long long k = 0; k < kIncrements; ++k)
        {
            local += 1;
            asm volatile("" : "+r"(local));
        }
        gLocaux[i] = local;
    });
    // On additionne seulement après les Join (faits dans Chronometrer).
    total = 0;
    for (int i = 0; i < kFils; ++i)
    {
        total += gLocaux[i];
    }
    return ms;
}

static double Mediane(double a, double b, double c)
{
    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

static void Serie(const char* nom, double (*mesurer)(long long&))
{
    double t[kMesures];
    long long total[kMesures];
    for (int m = 0; m < kMesures; ++m)
    {
        t[m] = mesurer(total[m]);
        std::printf("  %s mesure %d : %.3f ms | total %lld (attendu %lld)\n",
                    nom, m + 1, t[m], total[m], kAttendu);
    }
    std::printf("  %s MEDIANE : %.3f ms\n\n", nom, Mediane(t[0], t[1], t[2]));
}

int main()
{
    std::printf("%d fils, %lld increments chacun\n\n", kFils, kIncrements);
    Serie("sans protection", MesurerNu);
    Serie("mutex          ", MesurerMutex);
    Serie("atomique       ", MesurerAtomique);
    Serie("local          ", MesurerLocal);
    return 0;
}
