// L'étreinte fatale : deux fils, deux verrous, pris dans des ordres opposés.

#include <NKThreading/NkThread.h>
#include <NKThreading/NkMutex.h>
#include <NKThreading/NkScopedLock.h>
#include <NKTime/NkChrono.h>

#include <cstdio>
#include <new>

using namespace nkentseu;
using namespace nkentseu::threading;

static NkMutex m1;
static NkMutex m2;
// Version qui se fige (gardée en commentaire) : le fil a attend m2 ligne 26, le fil b attend m1 ligne 40.
// // Fil a : prend m1, attend, puis veut m2.
// static void FilA()
// {
//     threading::NkScopedLock<NkMutex> v1(m1);
//     std::printf("a : tient m1\n");
//     std::fflush(stdout);
//     NkChrono::SleepMilliseconds(100);
//     std::printf("a : veut m2\n");
//     std::fflush(stdout);
//     threading::NkScopedLock<NkMutex> v2(m2);
//     std::printf("a : tient m1 et m2\n");
//     std::fflush(stdout);
// }
//
// // Fil b : prend m2, attend, puis veut m1 (ordre opposé à celui du fil a).
// static void FilB()
// {
//     threading::NkScopedLock<NkMutex> v2(m2);
//     std::printf("b : tient m2\n");
//     std::fflush(stdout);
//     NkChrono::SleepMilliseconds(100);
//     std::printf("b : veut m1\n");
//     std::fflush(stdout);
//     threading::NkScopedLock<NkMutex> v1(m1);
//     std::printf("b : tient m1 et m2\n");
//     std::fflush(stdout);
// }
//
// int main()
// {
//     NkThread a([](void*) { FilA(); });
//     NkThread b([](void*) { FilB(); });
//     a.Join();
//     b.Join();
//     std::printf("fini : les deux fils sont joints\n");
//     return 0;
// }

// ===== Version corrigée : les deux fils prennent m1 puis m2, dans le même ordre =====

// Fil a : prend m1, attend, puis prend m2.
static void FilA()
{
    threading::NkScopedLock<NkMutex> v1(m1);
    std::printf("a : tient m1\n");
    std::fflush(stdout);
    NkChrono::SleepMilliseconds(100);
    std::printf("a : veut m2\n");
    std::fflush(stdout);
    threading::NkScopedLock<NkMutex> v2(m2);
    std::printf("a : tient m1 et m2\n");
    std::fflush(stdout);
}

// Fil b : prend aussi m1 d'abord, attend, puis prend m2 (même ordre que le fil a).
static void FilB()
{
    threading::NkScopedLock<NkMutex> v1(m1);
    std::printf("b : tient m1\n");
    std::fflush(stdout);
    NkChrono::SleepMilliseconds(100);
    std::printf("b : veut m2\n");
    std::fflush(stdout);
    threading::NkScopedLock<NkMutex> v2(m2);
    std::printf("b : tient m1 et m2\n");
    std::fflush(stdout);
}

int main()
{
    NkThread a([](void*) { FilA(); });
    NkThread b([](void*) { FilB(); });
    a.Join();
    b.Join();
    std::printf("fini : les deux fils sont joints\n");
    return 0;
}
