// La découverte : un serveur diffuse sa présence sur le réseau local,
// un client écoute et liste les serveurs trouvés.
//   Decouverte.exe serveur [nom] [port] [secondes]
//   Decouverte.exe client

#include <NKNetwork/NKNetwork.h>
#include <NKTime/NkChrono.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::net;

// Serveur : remplit une description, puis la diffuse toutes les deux secondes.
static int Serveur(const char* nom, uint16 port, int secondes)
{
    NkDiscovery::ServerInfo info;
    info.addr = NkAddress::Any(port);               // adresse de connexion du serveur
    std::strncpy(info.name, nom, sizeof(info.name) - 1);
    info.maxPlayers = 8;                            // 0 (défaut) serait écarté sans message

    NkChrono horloge;
    int diffusions = 0;
    while (horloge.Elapsed().milliseconds < secondes * 1000.0)
    {
        const NkNetResult r = NkDiscovery::Broadcast(info);
        ++diffusions;
        std::printf("[serveur %s] diffusion %d (code %d)\n", nom, diffusions, static_cast<int>(r));
        std::fflush(stdout);
        NkChrono::SleepMilliseconds(2000);
    }
    return 0;
}

// Client : Listen bloque pendant toute la durée d'écoute (6 secondes ici).
static int Client()
{
    NkVector<NkDiscovery::ServerInfo> trouves;
    std::printf("[client] ecoute pendant 6 secondes...\n");
    std::fflush(stdout);
    const NkNetResult r = NkDiscovery::Listen(6000, trouves);
    std::printf("[client] Listen a rendu le code %d\n", static_cast<int>(r));

    int n = 0;
    for (const auto& s : trouves)
    {
        ++n;
        std::printf("  adresse : %s | nom : %s | joueurs max : %u\n",
                    s.addr.ToString().CStr(), s.name, static_cast<unsigned>(s.maxPlayers));
    }
    std::printf("[client] %d entree(s) dans la liste\n", n);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::printf("usage : Decouverte.exe serveur [nom] [port] [secondes]\n"
                    "        Decouverte.exe client\n");
        return 1;
    }

    const NkNetResult init = NkSocket::PlatformInit();
    if (init != NkNetResult::NK_NET_OK)
    {
        std::printf("PlatformInit a echoue (code %d)\n", static_cast<int>(init));
        return 1;
    }

    int code = 1;
    if (std::strcmp(argv[1], "serveur") == 0)
    {
        const char* nom = argc > 2 ? argv[2] : "Serveur A";
        const uint16 port = static_cast<uint16>(argc > 3 ? std::atoi(argv[3]) : 7777);
        const int secondes = argc > 4 ? std::atoi(argv[4]) : 30;
        code = Serveur(nom, port, secondes);
    }
    else if (std::strcmp(argv[1], "client") == 0)
    {
        code = Client();
    }
    else
    {
        std::printf("role inconnu : %s (attendu : serveur ou client)\n", argv[1]);
    }

    NkSocket::PlatformShutdown();
    return code;
}
