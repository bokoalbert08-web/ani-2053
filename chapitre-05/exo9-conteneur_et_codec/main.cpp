#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace {

using Octets = std::vector<int>;

int chiffre(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    return std::tolower(static_cast<unsigned char>(c)) - 'a' + 10;
}

// Deux chiffres hexadecimaux par octet, majuscules comme minuscules. Un tiret : aucun octet.
Octets decoder(const std::string &hex) {
    Octets octets;
    if (hex == "-") {
        return octets;
    }
    for (std::size_t i = 0; i + 1 < hex.size(); i += 2) {
        octets.push_back(chiffre(hex[i]) * 16 + chiffre(hex[i + 1]));
    }
    return octets;
}

// Vrai si les octets donnes contiennent le motif a partir de `debut`.
// Un octet absent de la ligne fait que la regle ne s'applique pas.
bool commencePar(const Octets &octets, std::size_t debut, const Octets &motif) {
    if (octets.size() < debut + motif.size()) {
        return false;
    }
    for (std::size_t i = 0; i < motif.size(); ++i) {
        if (octets[debut + i] != motif[i]) {
            return false;
        }
    }
    return true;
}

// Le conteneur se reconnait aux octets, jamais a l'extension. Les regles sont testees dans
// l'ordre ; la premiere qui s'applique decide.
std::string conteneur(const Octets &b) {
    if (commencePar(b, 4, {0x66, 0x74, 0x79, 0x70}) && b.size() >= 12) {
        return "MP4";
    }
    if (commencePar(b, 0, {0x1A, 0x45, 0xDF, 0xA3})) {
        return "WEBM";
    }
    if (b.size() >= 12 && commencePar(b, 0, {0x52, 0x49, 0x46, 0x46}) &&
        commencePar(b, 8, {0x57, 0x41, 0x56, 0x45})) {
        return "WAV";
    }
    if (commencePar(b, 0, {0x4F, 0x67, 0x67, 0x53})) {
        return "OGG";
    }
    if (commencePar(b, 0, {0x66, 0x4C, 0x61, 0x43})) {
        return "FLAC";
    }
    if (commencePar(b, 0, {0x49, 0x44, 0x33})) {
        return "MP3";
    }
    if (b.size() >= 2 && b[0] == 0xFF && b[1] >= 0xE0) {
        return "MP3";
    }
    return "INCONNU";
}

// Un code absent de la table garde son nom de quatre caracteres, tel quel.
std::string codec(const std::string &code) {
    static const std::map<std::string, std::string> table = {
        {"mp4a", "aac"},  {"Opus", "opus"}, {"opus", "opus"}, {"avc1", "h264"}, {"avc3", "h264"},
        {"hvc1", "h265"}, {"hev1", "h265"}, {"vp08", "vp8"},  {"vp09", "vp9"},  {"mp4v", "mpeg4"},
        {".mp3", "mp3"},  {"twos", "pcm"},  {"sowt", "pcm"},  {"lpcm", "pcm"},
    };
    const auto trouve = table.find(code);
    return trouve == table.end() ? code : trouve->second;
}

// Nommer un codec n'est pas savoir le decoder : le lecteur a sa propre liste.
bool lisible(const std::string &code) {
    return code == "mjpa" || code == "jpeg" || code == "MJPG" || code == "avc1" ||
           code == "avc3" || code == "hvc1" || code == "hev1" || code == "av01";
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int nombre = 0;
    std::cin >> nombre;

    long long mp4 = 0;
    long long lisibles = 0;
    long long inconnus = 0;

    std::string nom;
    std::string hex;
    int pistes = 0;
    for (int i = 0; i < nombre && std::cin >> nom >> hex >> pistes; ++i) {
        // Les pistes sont toujours lues, meme quand elles sont ignorees : sinon la ligne
        // suivante serait lue de travers.
        std::vector<std::string> types;
        std::vector<std::string> codes;
        for (int p = 0; p < pistes; ++p) {
            std::string type;
            std::string code;
            std::cin >> type >> code;
            types.push_back(type);
            codes.push_back(code);
        }

        const std::string boite = conteneur(decoder(hex));
        std::cout << nom << ' ' << boite << '\n';

        if (boite == "INCONNU") {
            ++inconnus;
        }
        if (boite != "MP4") {
            continue;
        }

        ++mp4;
        std::string verdict = "SANS_IMAGE";
        bool imageTrouvee = false;
        for (std::size_t p = 0; p < types.size(); ++p) {
            const bool video = types[p] == "vide";
            std::cout << nom << " PISTE " << p + 1 << ' ' << (video ? "VIDEO" : "AUDIO") << ' '
                      << codec(codes[p]) << '\n';
            // Le lecteur ne juge que la premiere piste vide.
            if (video && !imageTrouvee) {
                imageTrouvee = true;
                verdict = lisible(codes[p]) ? "LISIBLE" : "ECHEC";
            }
        }
        std::cout << nom << " LECTEUR " << verdict << '\n';
        if (verdict == "LISIBLE") {
            ++lisibles;
        }
    }

    std::cout << "MP4 " << mp4 << '\n';
    std::cout << "LISIBLES " << lisibles << '\n';
    std::cout << "INCONNUS " << inconnus << '\n';
    return 0;
}
