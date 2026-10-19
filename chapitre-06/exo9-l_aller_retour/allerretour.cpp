// L'aller-retour : le client mesure cent fois le temps d'un aller-retour avec
// le serveur, le lisse comme un protocole fiable, et lit l'estimation du moteur.
//   AllerRetour.exe serveur
//   AllerRetour.exe client [adresse]      (127.0.0.1 par défaut)

#include <NKNetwork/NKNetwork.h>
#include <NKTime/NkChrono.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace nkentseu;
using namespace nkentseu::net;

static const uint16 kPort = 7777;
static const int kEssais = 100;

// Serveur : renvoie chaque message reçu à son expéditeur (le champ from), tel quel.
static int Serveur(int pauseMs, int canal)
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

    // Interrogation en boucle serrée, sans pause : une pause ajouterait son
    // propre retard à l'aller-retour mesuré.
    NkChrono horloge;
    double dernier = -1.0;
    int renvoyes = 0;
    while (horloge.Elapsed().milliseconds < 180000.0)
    {
        NkVector<NkReceiveMsg> recus;
        serveur.DrainAll(recus);
        for (const auto& msg : recus)
        {
            (void)serveur.SendTo(msg.from, reinterpret_cast<const uint8*>(msg.data), msg.size,
                                 static_cast<NkNetChannel>(canal));
            ++renvoyes;
            dernier = horloge.Elapsed().milliseconds;
        }
        if (dernier >= 0.0 && horloge.Elapsed().milliseconds - dernier > 3000.0)
        {
            break;
        }
        if (pauseMs > 0)
        {
            NkChrono::SleepMilliseconds(pauseMs);
        }
    }
    std::printf("[serveur] %d message(s) renvoye(s)\n", renvoyes);
    serveur.DisconnectAll("fin de l'essai");
    serveur.Shutdown();
    return 0;
}

// Client : un message numéroté à la fois, on attend le même numéro en retour.
static int Client(const char* adresse, int pauseMs, int canal)
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
    NkPeerId serveurId;
    serveurId.value = idServeur.load();
    std::printf("[client] connecte au serveur (id %llu)\n", static_cast<unsigned long long>(serveurId.value));

    double mesures[kEssais] = {};
    for (int i = 0; i < kEssais; ++i)
    {
        const uint32 numero = static_cast<uint32>(i + 1);
        NkChrono chrono;
        const NkNetResult r = client.SendTo(serveurId, reinterpret_cast<const uint8*>(&numero), sizeof(numero),
                                               static_cast<NkNetChannel>(canal));
        if (r != NkNetResult::NK_NET_OK)
        {
            std::printf("[client] SendTo a echoue au message %u (code %d)\n", numero, static_cast<int>(r));
            client.Shutdown();
            return 1;
        }
        bool recu = false;
        while (!recu && chrono.Elapsed().milliseconds < 2000.0)
        {
            NkVector<NkReceiveMsg> recus;
            client.DrainAll(recus);
            for (const auto& msg : recus)
            {
                uint32 recuNumero = 0;
                if (msg.size >= sizeof(recuNumero))
                {
                    std::memcpy(&recuNumero, msg.data, sizeof(recuNumero));
                    if (recuNumero == numero)
                    {
                        mesures[i] = chrono.Elapsed().milliseconds;
                        recu = true;
                    }
                }
            }
            if (!recu && pauseMs > 0)
            {
                NkChrono::SleepMilliseconds(pauseMs);
            }
        }
        if (!recu)
        {
            std::printf("[client] pas de retour pour le message %u apres 2 secondes\n", numero);
            client.Shutdown();
            return 1;
        }
    }

    // Les cent mesures, une par ligne, sans autre texte.
    std::FILE* f = std::fopen("mesures-rtt.txt", "w");
    if (f != nullptr)
    {
        for (int i = 0; i < kEssais; ++i)
        {
            std::fprintf(f, "%.3f\n", mesures[i]);
        }
        std::fclose(f);
    }

    // Moyenne, minimum, maximum, et valeur lissée : SRTT = (1 - a) * SRTT + a * R,
    // avec a = 0,125 et un départ à 100 ms (les valeurs du moteur).
    double somme = 0.0;
    double min = mesures[0];
    double max = mesures[0];
    double srtt = 100.0;
    for (int i = 0; i < kEssais; ++i)
    {
        somme += mesures[i];
        if (mesures[i] < min) { min = mesures[i]; }
        if (mesures[i] > max) { max = mesures[i]; }
        srtt = (1.0 - 0.125) * srtt + 0.125 * mesures[i];
    }
    std::printf("Moyenne : %.3f ms\n", somme / kEssais);
    std::printf("Minimum : %.3f ms\n", min);
    std::printf("Maximum : %.3f ms\n", max);
    std::printf("Lissee : %.3f ms\n", srtt);

    // L'estimation du moteur.
    NkConnectionStats stats;
    if (client.GetConnectionStats(serveurId, stats))
    {
        std::printf("Moteur : %.3f ms\n", static_cast<double>(stats.rttMs));
    }
    else
    {
        std::printf("Moteur : GetConnectionStats n'a pas rendu de statistiques\n");
    }
    std::fflush(stdout);

    NkChrono::SleepMilliseconds(500);
    client.Shutdown();
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::printf("usage : AllerRetour.exe serveur\n        AllerRetour.exe client [adresse]\n");
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
        code = Serveur(argc > 2 ? std::atoi(argv[2]) : 0, argc > 3 ? std::atoi(argv[3]) : 1);
    }
    else if (std::strcmp(argv[1], "client") == 0)
    {
        code = Client(argc > 2 ? argv[2] : "127.0.0.1", argc > 3 ? std::atoi(argv[3]) : 0,
                      argc > 4 ? std::atoi(argv[4]) : 1);
    }
    else
    {
        std::printf("role inconnu : %s\n", argv[1]);
    }
    NkSocket::PlatformShutdown();
    return code;
}
