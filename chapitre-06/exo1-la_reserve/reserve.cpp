// La réserve : deux cents tâches courtes, soumises à une réserve de fils
// puis confiées à deux cents fils créés à la main. On mesure les deux.

#include <NKThreading/NkThreadPool.h>
#include <NKThreading/NkThread.h>
#include <NKTime/NkChrono.h>

#include <cstdio>
#include <vector>

using namespace nkentseu;
using namespace nkentseu::threading;

static const int kTaches = 200;
static const int kRepetitions = 3;

// La tâche : additionner `n` entiers dans une variable locale.
// Le résultat est rangé dans une case propre à la tâche : personne ne
// l'écrase, et le compilateur ne peut pas retirer le calcul.
static void Tache(long long* sortie, int n)
{
    long long somme = 0;
    for (int i = 1; i <= n; ++i)
    {
        somme += i;
    }
    *sortie = somme;
}

// Version réserve : deux cents Enqueue, puis un seul Join.
static double MesurerReserve(NkThreadPool& reserve, std::vector<long long>& r, int n)
{
    NkChrono chrono;
    for (int i = 0; i < kTaches; ++i)
    {
        long long* sortie = &r[i];
        reserve.Enqueue([sortie, n]() { Tache(sortie, n); });
    }
    reserve.Join();
    return chrono.Elapsed().milliseconds;
}

// Version fils : un NkThread par tâche, puis Join sur chacun des deux cents.
static double MesurerFils(std::vector<long long>& r, int n)
{
    NkChrono chrono;
    std::vector<NkThread> fils;
    fils.reserve(kTaches);
    for (int i = 0; i < kTaches; ++i)
    {
        long long* sortie = &r[i];
        fils.emplace_back([sortie, n](void*) { Tache(sortie, n); });
    }
    for (int i = 0; i < kTaches; ++i)
    {
        fils[i].Join();
    }
    return chrono.Elapsed().milliseconds;
}

static long long Total(const std::vector<long long>& r)
{
    long long t = 0;
    for (size_t i = 0; i < r.size(); ++i)
    {
        t += r[i];
    }
    return t;
}

static void Serie(NkThreadPool& reserve, const char* nom, int n)
{
    std::vector<long long> resultats(kTaches, 0);
    std::printf("%s (%d entiers par tache)\n", nom, n);
    for (int m = 1; m <= kRepetitions; ++m)
    {
        double tr = MesurerReserve(reserve, resultats, n);
        std::printf("  reserve mesure %d : %.3f ms (somme %lld)\n", m, tr, Total(resultats));
    }
    for (int m = 1; m <= kRepetitions; ++m)
    {
        double tf = MesurerFils(resultats, n);
        std::printf("  fils    mesure %d : %.3f ms (somme %lld)\n", m, tf, Total(resultats));
    }
}

int main()
{
    NkThreadPool reserve;
    std::printf("Ouvriers de la reserve : %u\n", (unsigned)reserve.GetNumWorkers());

    Serie(reserve, "Tache courte", 1000);
    Serie(reserve, "Tache longue", 100000);   // cent fois plus longue
    return 0;
}
