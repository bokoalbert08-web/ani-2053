// Deux machines se parlent : un serveur et un client NKNetwork.
// Un seul programme, deux rôles choisis par un argument :
//   Parle.exe serveur
//   Parle.exe client [adresse-du-serveur]     (127.0.0.1 par défaut)

#include <NKNetwork/NKNetwork.h>
#include <NKTime/NkChrono.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::net;

static const uint16 kPort = 7777;

static unsigned long long Valeur(const NkPeerId& pair)
{
    return static_cast<unsigned long long>(pair.value);
}

// Serveur : écoute sur le port, affiche chaque pair qui se connecte et chaque
// message reçu avec l'identifiant de son expéditeur (le champ from).
static int Serveur()
{
    NkConnectionManager serveur;
    serveur.onPeerConnected = [](NkPeerId pair)
    {
        std::printf("[serveur] pair connecte : id %llu\n", Valeur(pair));
        std::fflush(stdout);
    };

    const NkNetResult r = serveur.StartServer(kPort, 8);
    if (r != NkNetResult::NK_NET_OK)
    {
        std::printf("[serveur] StartServer a echoue (code %d)\n", static_cast<int>(r));
        return 1;
    }
    std::printf("[serveur] ecoute sur le port %u\n", static_cast<unsigned>(kPort));
    std::fflush(stdout);

    // DrainAll à chaque tour : sans lui, les messages restent en file.
    NkChrono horloge;
    double dernierMessage = -1.0;
    while (horloge.Elapsed().milliseconds < 120000.0)
    {
        NkVector<NkReceiveMsg> recus;
        serveur.DrainAll(recus);
        for (const auto& msg : recus)
        {
            std::printf("[serveur] message de l'id %llu : %.*s\n",
                        Valeur(msg.from), static_cast<int>(msg.size),
                        reinterpret_cast<const char*>(msg.data));
            std::fflush(stdout);
            dernierMessage = horloge.Elapsed().milliseconds;
        }
        // On s'arrête 2 secondes après le dernier message reçu.
        if (dernierMessage >= 0.0 && horloge.Elapsed().milliseconds - dernierMessage > 2000.0)
        {
            break;
        }
        NkChrono::SleepMilliseconds(10);
    }

    serveur.DisconnectAll("fin de l'essai");
    serveur.Shutdown();
    return 0;
}

// Client : se connecte, attend d'être connecté (la connexion se fait dans un
// fil du moteur, pas dans cette boucle), puis envoie un court message.
static int Client(const char* adresse)
{
    NkConnectionManager client;
    std::atomic<unsigned long long> idServeur(0);
    client.onPeerConnected = [&idServeur](NkPeerId pair)
    {
        std::printf("[client] pair connecte : id %llu\n", Valeur(pair));
        std::fflush(stdout);
        idServeur = Valeur(pair);
    };

    std::printf("[client] connexion a %s:%u\n", adresse, static_cast<unsigned>(kPort));
    std::fflush(stdout);
    const NkNetResult r = client.Connect(NkAddress(adresse, kPort));
    if (r != NkNetResult::NK_NET_OK)
    {
        std::printf("[client] Connect a echoue (code %d)\n", static_cast<int>(r));
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

    const char* texte = "bonjour serveur";
    NkPeerId serveurId;
    serveurId.value = idServeur.load();
    const NkNetResult envoi = client.SendTo(
        serveurId, reinterpret_cast<const uint8*>(texte),
        static_cast<uint32>(std::strlen(texte)));
    std::printf("[client] message envoye \"%s\" (code %d)\n", texte, static_cast<int>(envoi));
    std::fflush(stdout);

    // On laisse au fil du moteur le temps d'envoyer avant de tout fermer.
    NkChrono::SleepMilliseconds(1000);
    client.Shutdown();
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::printf("usage : Parle.exe serveur\n        Parle.exe client [adresse]\n");
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
        code = Serveur();
    }
    else if (std::strcmp(argv[1], "client") == 0)
    {
        code = Client(argc > 2 ? argv[2] : "127.0.0.1");
    }
    else
    {
        std::printf("role inconnu : %s (attendu : serveur ou client)\n", argv[1]);
    }

    NkSocket::PlatformShutdown();
    return code;
}
