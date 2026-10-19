// Le joueur fantôme : un serveur tient la table de ses joueurs, retire de la
// table un joueur qui se déconnecte, et mesure le temps mis à s'en apercevoir.
//   Fantome.exe serveur [nombre-de-deconnexions]       (4 par défaut)
//   Fantome.exe client [adresse] coupure               envoie jusqu'à ce qu'on le tue
//   Fantome.exe client [adresse] propre                envoie 3 s puis Shutdown()

#include <NKNetwork/NKNetwork.h>
#include <NKThreading/NkMutex.h>
#include <NKThreading/NkScopedLock.h>
#include <NKTime/NkChrono.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <thread>

using namespace nkentseu;
using namespace nkentseu::net;

static const uint16 kPort = 7777;

// La table des joueurs : touchée par le fil réseau (rappels) et par la boucle
// principale (messages reçus). Toujours sous le verrou gVerrou.
struct Joueur
{
    unsigned long long id;
    double dernierMessage;   // instant du dernier message reçu, en ms
};
static const int kMaxJoueurs = 16;
static Joueur gTable[kMaxJoueurs];
static int gNombre = 0;
static threading::NkMutex gVerrou;

static NkChrono gHorloge;
static std::atomic<int> gDeconnexions(0);
static std::thread::id gFilPrincipal;

static unsigned long long IdFil()
{
    return static_cast<unsigned long long>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

static double Maintenant()
{
    return gHorloge.Elapsed().milliseconds;
}

// À appeler verrou pris.
static void AfficherTable(const char* quand)
{
    std::printf("[table] %s : %d joueur(s)", quand, gNombre);
    for (int i = 0; i < gNombre; ++i)
    {
        std::printf(" [id %llu]", gTable[i].id);
    }
    std::printf("\n");
    std::fflush(stdout);
}

static void Serveur(int attendues)
{
    gFilPrincipal = std::this_thread::get_id();
    std::printf("[serveur] fil de la boucle principale : %llu\n", IdFil());

    NkConnectionManager serveur;

    // Rappel de connexion : appelé depuis le fil réseau du moteur.
    serveur.onPeerConnected = [](NkPeerId pair)
    {
        threading::NkScopedLock<threading::NkMutex> verrou(gVerrou);
        if (gNombre < kMaxJoueurs)
        {
            gTable[gNombre].id = static_cast<unsigned long long>(pair.value);
            gTable[gNombre].dernierMessage = Maintenant();
            ++gNombre;
        }
        std::printf("[serveur] connexion du joueur %llu (rappel dans le fil %llu, boucle principale : %s)\n",
                    static_cast<unsigned long long>(pair.value), IdFil(),
                    std::this_thread::get_id() == gFilPrincipal ? "oui" : "non");
        AfficherTable("apres connexion");
    };

    // Rappel de déconnexion : on retire le joueur, on note la raison et le
    // temps écoulé depuis son dernier message.
    serveur.onPeerDisconnected = [](NkPeerId pair, const char* raison)
    {
        threading::NkScopedLock<threading::NkMutex> verrou(gVerrou);
        const unsigned long long id = static_cast<unsigned long long>(pair.value);
        double depuis = -1.0;
        for (int i = 0; i < gNombre; ++i)
        {
            if (gTable[i].id == id)
            {
                depuis = Maintenant() - gTable[i].dernierMessage;
                gTable[i] = gTable[gNombre - 1];
                --gNombre;
                break;
            }
        }
        std::printf("[serveur] deconnexion du joueur %llu : raison \"%s\", %.1f ms depuis son dernier message\n",
                    id, raison != nullptr ? raison : "(aucune)", depuis);
        std::printf("[serveur] rappel de deconnexion dans le fil %llu (boucle principale : %s)\n",
                    IdFil(), std::this_thread::get_id() == gFilPrincipal ? "oui" : "non");
        AfficherTable("apres deconnexion");
        ++gDeconnexions;
    };

    if (serveur.StartServer(kPort, 8) != NkNetResult::NK_NET_OK)
    {
        std::printf("[serveur] StartServer a echoue\n");
        return;
    }
    std::printf("[serveur] ecoute sur le port %u, arret apres %d deconnexion(s)\n",
                static_cast<unsigned>(kPort), attendues);
    std::fflush(stdout);

    double fin = -1.0;
    while (Maintenant() < 600000.0)
    {
        NkVector<NkReceiveMsg> recus;
        serveur.DrainAll(recus);
        for (const auto& msg : recus)
        {
            threading::NkScopedLock<threading::NkMutex> verrou(gVerrou);
            for (int i = 0; i < gNombre; ++i)
            {
                if (gTable[i].id == static_cast<unsigned long long>(msg.from.value))
                {
                    gTable[i].dernierMessage = Maintenant();
                }
            }
        }
        if (fin < 0.0 && gDeconnexions.load() >= attendues)
        {
            fin = Maintenant();
        }
        if (fin >= 0.0 && Maintenant() - fin > 1000.0)
        {
            break;
        }
        NkChrono::SleepMilliseconds(5);
    }
    serveur.DisconnectAll("fin de l'essai");
    serveur.Shutdown();
}

static int Client(const char* adresse, bool propre)
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
    NkChrono attente;
    while (idServeur.load() == 0 && attente.Elapsed().milliseconds < 5000.0)
    {
        NkChrono::SleepMilliseconds(20);
    }
    if (idServeur.load() == 0)
    {
        std::printf("[client] pas de connexion apres 5 secondes\n");
        client.Shutdown();
        return 1;
    }
    std::printf("[client] connecte, envoi d'un message toutes les 100 ms (%s)\n",
                propre ? "fermeture propre apres 3 s" : "tuez ce processus pour couper");
    std::fflush(stdout);

    NkPeerId serveurId;
    serveurId.value = idServeur.load();
    NkChrono duree;
    uint32 numero = 0;
    while (!propre || duree.Elapsed().milliseconds < 3000.0)
    {
        ++numero;
        (void)client.SendTo(serveurId, reinterpret_cast<const uint8*>(&numero), sizeof(numero));
        NkChrono::SleepMilliseconds(100);
    }
    std::printf("[client] %u messages envoyes, appel de Shutdown()\n", numero);
    std::fflush(stdout);
    client.Shutdown();
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::printf("usage : Fantome.exe serveur [nombre-de-deconnexions]\n"
                    "        Fantome.exe client [adresse] coupure|propre\n");
        return 1;
    }
    if (NkSocket::PlatformInit() != NkNetResult::NK_NET_OK)
    {
        std::printf("PlatformInit a echoue\n");
        return 1;
    }
    int code = 1;
    if (std::strcmp(argv[1], "serveur") == 0)
    {
        Serveur(argc > 2 ? std::atoi(argv[2]) : 4);
        code = 0;
    }
    else if (std::strcmp(argv[1], "client") == 0)
    {
        const char* adresse = argc > 2 ? argv[2] : "127.0.0.1";
        const bool propre = argc > 3 && std::strcmp(argv[3], "propre") == 0;
        code = Client(adresse, propre);
    }
    else
    {
        std::printf("role inconnu : %s\n", argv[1]);
    }
    NkSocket::PlatformShutdown();
    return code;
}
