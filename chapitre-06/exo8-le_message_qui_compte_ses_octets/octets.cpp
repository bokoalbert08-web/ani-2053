// Le message qui compte ses octets : le même état de joueur écrit deux fois,
// en types complets puis quantifié, avec un NkBitWriter ; relu par un NkBitReader.
//   Octets.exe local                      tailles et écarts, sans réseau
//   Octets.exe serveur                    reçoit les deux paquets et les relit
//   Octets.exe client [adresse]           écrit les deux paquets et les envoie

#include <NKNetwork/NKNetwork.h>
#include <NKTime/NkChrono.h>

#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::net;

static const uint16 kPort = 7777;

// Bornes et précisions de l'énoncé : les mêmes à l'écriture et à la lecture.
static const float kPosMin = -500.f;
static const float kPosMax = 500.f;
static const float kPosPrec = 0.01f;     // au centimètre
static const float kAngMin = 0.f;
static const float kAngMax = 360.f;
static const float kAngPrec = 0.5f;      // au demi-degré
static const int32 kSanteMin = 0;
static const int32 kSanteMax = 100;
static const uint32 kCapacite = 64;

struct EtatJoueur
{
    bool vivant;
    uint8 numero;
    float x;
    float y;
    float angle;
    int32 sante;
};

static const EtatJoueur kExemple = {true, 42, 123.456f, -87.654f, 271.3f, 73};

// Paquet complet : chaque champ dans son type entier.
static uint32 EcrireComplet(const EtatJoueur& e, uint8* buf, uint32* bits)
{
    NkBitWriter w(buf, kCapacite);
    w.WriteU8(e.vivant ? 1 : 0);
    w.WriteU8(e.numero);
    w.WriteF32(e.x);
    w.WriteF32(e.y);
    w.WriteF32(e.angle);
    w.WriteI32(e.sante);
    if (bits != nullptr) { *bits = w.BitsWritten(); }
    return w.IsOverflowed() ? 0 : w.BytesWritten();
}

// Paquet quantifié : un bit pour l'état, bornes et précisions pour le reste.
static uint32 EcrireQuantifie(const EtatJoueur& e, uint8* buf, uint32* bits)
{
    NkBitWriter w(buf, kCapacite);
    w.WriteBool(e.vivant);
    w.WriteU8(e.numero);
    w.WriteF32Q(e.x, kPosMin, kPosMax, kPosPrec);
    w.WriteF32Q(e.y, kPosMin, kPosMax, kPosPrec);
    w.WriteF32Q(e.angle, kAngMin, kAngMax, kAngPrec);
    w.WriteInt(e.sante, kSanteMin, kSanteMax);
    if (bits != nullptr) { *bits = w.BitsWritten(); }
    return w.IsOverflowed() ? 0 : w.BytesWritten();
}

// Relectures : même ordre, mêmes bornes, même précision.
static bool LireComplet(const uint8* buf, uint32 taille, EtatJoueur& e)
{
    NkBitReader r(buf, taille);
    e.vivant = r.ReadU8() != 0;
    e.numero = r.ReadU8();
    e.x = r.ReadF32();
    e.y = r.ReadF32();
    e.angle = r.ReadF32();
    e.sante = r.ReadI32();
    return !r.IsOverflowed();
}

static bool LireQuantifie(const uint8* buf, uint32 taille, EtatJoueur& e)
{
    NkBitReader r(buf, taille);
    e.vivant = r.ReadBool();
    e.numero = r.ReadU8();
    e.x = r.ReadF32Q(kPosMin, kPosMax, kPosPrec);
    e.y = r.ReadF32Q(kPosMin, kPosMax, kPosPrec);
    e.angle = r.ReadF32Q(kAngMin, kAngMax, kAngPrec);
    e.sante = r.ReadInt(kSanteMin, kSanteMax);
    return !r.IsOverflowed();
}

static void Afficher(const char* titre, const EtatJoueur& e)
{
    std::printf("  %s : vivant=%d numero=%u x=%.4f y=%.4f angle=%.4f sante=%d\n",
                titre, e.vivant ? 1 : 0, static_cast<unsigned>(e.numero),
                e.x, e.y, e.angle, static_cast<int>(e.sante));
}

// Mode local : tailles de l'exemple, puis écarts maximaux sur 200000 états
// tirés au hasard dans les bornes.
static int Local()
{
    uint8 buf[kCapacite] = {};
    uint32 bitsC = 0;
    uint32 bitsQ = 0;
    const uint32 octetsC = EcrireComplet(kExemple, buf, &bitsC);
    EtatJoueur relu = {};
    const bool okC = LireComplet(buf, octetsC, relu);
    std::printf("Paquet complet   : %u octets (%u bits), relecture %s\n",
                static_cast<unsigned>(octetsC), static_cast<unsigned>(bitsC), okC ? "ok" : "ECHEC");
    Afficher("envoye", kExemple);
    Afficher("relu  ", relu);

    uint8 bufQ[kCapacite] = {};
    const uint32 octetsQ = EcrireQuantifie(kExemple, bufQ, &bitsQ);
    EtatJoueur reluQ = {};
    const bool okQ = LireQuantifie(bufQ, octetsQ, reluQ);
    std::printf("Paquet quantifie : %u octets (%u bits), relecture %s\n",
                static_cast<unsigned>(octetsQ), static_cast<unsigned>(bitsQ), okQ ? "ok" : "ECHEC");
    Afficher("envoye", kExemple);
    Afficher("relu  ", reluQ);

    // Écarts maximaux : complet (cp) et quantifié (q).
    double cpX = 0, cpY = 0, cpA = 0, cpS = 0;
    double qX = 0, qY = 0, qA = 0, qS = 0;
    int erreursEntiers = 0;
    uint32 graine = 12345u;
    auto hasard = [&graine]() -> float
    {
        graine = graine * 1664525u + 1013904223u;
        return static_cast<float>(graine >> 8) / 16777216.0f;
    };
    const int n = 200000;
    for (int i = 0; i < n; ++i)
    {
        EtatJoueur e;
        e.vivant = hasard() < 0.5f;
        e.numero = static_cast<uint8>(hasard() * 256.0f);
        e.x = kPosMin + hasard() * (kPosMax - kPosMin);
        e.y = kPosMin + hasard() * (kPosMax - kPosMin);
        e.angle = kAngMin + hasard() * (kAngMax - kAngMin);
        e.sante = static_cast<int32>(hasard() * 101.0f);
        if (e.sante > kSanteMax) { e.sante = kSanteMax; }

        uint8 b[kCapacite] = {};
        EtatJoueur r = {};
        const uint32 oc = EcrireComplet(e, b, nullptr);
        LireComplet(b, oc, r);
        cpX = std::fmax(cpX, std::fabs(static_cast<double>(r.x) - e.x));
        cpY = std::fmax(cpY, std::fabs(static_cast<double>(r.y) - e.y));
        cpA = std::fmax(cpA, std::fabs(static_cast<double>(r.angle) - e.angle));
        cpS = std::fmax(cpS, std::fabs(static_cast<double>(r.sante) - e.sante));
        if (r.vivant != e.vivant || r.numero != e.numero) { ++erreursEntiers; }

        uint8 bq[kCapacite] = {};
        EtatJoueur rq = {};
        const uint32 oq = EcrireQuantifie(e, bq, nullptr);
        LireQuantifie(bq, oq, rq);
        qX = std::fmax(qX, std::fabs(static_cast<double>(rq.x) - e.x));
        qY = std::fmax(qY, std::fabs(static_cast<double>(rq.y) - e.y));
        qA = std::fmax(qA, std::fabs(static_cast<double>(rq.angle) - e.angle));
        qS = std::fmax(qS, std::fabs(static_cast<double>(rq.sante) - e.sante));
        if (rq.vivant != e.vivant || rq.numero != e.numero) { ++erreursEntiers; }
    }

    std::printf("\nEcarts maximaux sur %d etats (vivant et numero : %d erreur(s))\n", n, erreursEntiers);
    std::printf("  champ      complet        quantifie\n");
    std::printf("  x          %.8f     %.8f\n", cpX, qX);
    std::printf("  y          %.8f     %.8f\n", cpY, qY);
    std::printf("  angle      %.8f     %.8f\n", cpA, qA);
    std::printf("  sante      %.8f     %.8f\n", cpS, qS);
    return 0;
}

// Serveur : relit chaque paquet reçu. Le paquet complet est bien plus gros
// que le quantifié : la taille suffit à les reconnaître.
static int Serveur()
{
    NkConnectionManager serveur;
    serveur.onPeerConnected = [](NkPeerId pair)
    {
        std::printf("[serveur] pair connecte : id %llu\n", static_cast<unsigned long long>(pair.value));
        std::fflush(stdout);
    };
    if (serveur.StartServer(kPort, 8) != NkNetResult::NK_NET_OK)
    {
        std::printf("[serveur] StartServer a echoue\n");
        return 1;
    }
    std::printf("[serveur] ecoute sur le port %u\n", static_cast<unsigned>(kPort));
    std::fflush(stdout);

    NkChrono horloge;
    double dernier = -1.0;
    while (horloge.Elapsed().milliseconds < 120000.0)
    {
        NkVector<NkReceiveMsg> recus;
        serveur.DrainAll(recus);
        for (const auto& msg : recus)
        {
            const uint8* data = reinterpret_cast<const uint8*>(msg.data);
            EtatJoueur e = {};
            const bool complet = msg.size >= 16;
            const bool ok = complet ? LireComplet(data, msg.size, e) : LireQuantifie(data, msg.size, e);
            std::printf("[serveur] paquet %s de %u octets, relecture %s\n",
                        complet ? "complet" : "quantifie", static_cast<unsigned>(msg.size), ok ? "ok" : "ECHEC");
            Afficher("relu", e);
            std::fflush(stdout);
            dernier = horloge.Elapsed().milliseconds;
        }
        if (dernier >= 0.0 && horloge.Elapsed().milliseconds - dernier > 2000.0) { break; }
        NkChrono::SleepMilliseconds(10);
    }
    serveur.DisconnectAll("fin de l'essai");
    serveur.Shutdown();
    return 0;
}

// Client : écrit les deux paquets et les envoie au serveur.
static int Client(const char* adresse)
{
    NkConnectionManager client;
    std::atomic<unsigned long long> idServeur(0);
    client.onPeerConnected = [&idServeur](NkPeerId pair)
    {
        idServeur = static_cast<unsigned long long>(pair.value);
    };
    if (client.Connect(NkAddress(adresse, kPort)) != NkNetResult::NK_NET_OK)
    {
        std::printf("[client] Connect a echoue\n");
        return 1;
    }
    NkChrono horloge;
    while (idServeur.load() == 0 && horloge.Elapsed().milliseconds < 5000.0)
    {
        NkChrono::SleepMilliseconds(20);
    }
    if (idServeur.load() == 0)
    {
        std::printf("[client] pas de connexion apres 5 secondes\n");
        client.Shutdown();
        return 1;
    }

    NkPeerId serveurId;
    serveurId.value = idServeur.load();
    uint8 bufC[kCapacite] = {};
    uint8 bufQ[kCapacite] = {};
    uint32 bitsC = 0;
    uint32 bitsQ = 0;
    const uint32 octetsC = EcrireComplet(kExemple, bufC, &bitsC);
    const uint32 octetsQ = EcrireQuantifie(kExemple, bufQ, &bitsQ);
    Afficher("envoye", kExemple);
    std::printf("[client] paquet complet : %u octets (%u bits)\n", static_cast<unsigned>(octetsC), static_cast<unsigned>(bitsC));
    std::printf("[client] paquet quantifie : %u octets (%u bits)\n", static_cast<unsigned>(octetsQ), static_cast<unsigned>(bitsQ));
    const NkNetResult r1 = client.SendTo(serveurId, bufC, octetsC);
    NkChrono::SleepMilliseconds(200);
    const NkNetResult r2 = client.SendTo(serveurId, bufQ, octetsQ);
    std::printf("[client] envois : complet code %d, quantifie code %d\n", static_cast<int>(r1), static_cast<int>(r2));
    std::fflush(stdout);
    NkChrono::SleepMilliseconds(1000);
    client.Shutdown();
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::printf("usage : Octets.exe local\n        Octets.exe serveur\n        Octets.exe client [adresse]\n");
        return 1;
    }
    if (std::strcmp(argv[1], "local") == 0)
    {
        return Local();
    }

    if (NkSocket::PlatformInit() != NkNetResult::NK_NET_OK)
    {
        std::printf("PlatformInit a echoue\n");
        return 1;
    }
    int code = 1;
    if (std::strcmp(argv[1], "serveur") == 0)
    {
        code = Serveur();
    }
    else if (std::strcmp(argv[1], "client") == 0)
    {
        code = Client(argc > 2 ? argv[2] : "127.0.0.1");
    }
    else
    {
        std::printf("role inconnu : %s\n", argv[1]);
    }
    NkSocket::PlatformShutdown();
    return code;
}
