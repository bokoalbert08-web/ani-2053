#include <cctype>
#include <iostream>
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

bool estBlanc(int octet) {
    return octet == 0x20 || octet == 0x09 || octet == 0x0A || octet == 0x0D;
}

// Les regles, dans l'ordre ; la premiere qui s'applique decide. "" : aucun format.
std::string reconnaitre(long long taille, const Octets &b) {
    if (taille < 4) {
        return "";
    }
    if (taille >= 8 && commencePar(b, 0, {0x89, 0x50, 0x4E, 0x47})) {
        return "PNG";
    }
    if (commencePar(b, 0, {0xFF, 0xD8, 0xFF})) {
        return "JPEG";
    }
    if (commencePar(b, 0, {0x42, 0x4D})) {
        return "BMP";
    }
    if (commencePar(b, 0, {0x71, 0x6F, 0x69, 0x66})) {
        return "QOI";
    }
    if (commencePar(b, 0, {0x47, 0x49, 0x46, 0x38})) {
        return "GIF";
    }
    if (b.size() >= 4 && b[0] == 0x00 && b[1] == 0x00 && (b[2] == 0x01 || b[2] == 0x02) &&
        b[3] == 0x00) {
        return "ICO";
    }
    if (taille >= 10 && commencePar(b, 0, {0x23, 0x3F})) {
        return "HDR";
    }
    if (commencePar(b, 0, {0x76, 0x2F, 0x31, 0x01})) {
        return "EXR";
    }
    if (b.size() >= 2 && b[0] == 0x50 && b[1] >= 0x31 && b[1] <= 0x36) {
        if (b[1] == 0x31 || b[1] == 0x34) {
            return "PBM";
        }
        return (b[1] == 0x32 || b[1] == 0x35) ? "PGM" : "PPM";
    }
    if (taille >= 18 && b.size() >= 3) {
        const int t = b[2];
        if (t == 0x00 || t == 0x01 || t == 0x02 || t == 0x03 || t == 0x09 || t == 0x0A ||
            t == 0x0B) {
            return "TGA";
        }
    }

    // SVG : on saute la marque d'ordre des octets, puis les blancs.
    std::size_t i = commencePar(b, 0, {0xEF, 0xBB, 0xBF}) ? 3 : 0;
    while (i < b.size() && estBlanc(b[i])) {
        ++i;
    }
    if (commencePar(b, i, {0x3C, 0x3F, 0x78, 0x6D, 0x6C}) ||
        commencePar(b, i, {0x3C, 0x73, 0x76, 0x67})) {
        return "SVG";
    }
    return "";
}

// Le texte apres le DERNIER point, en minuscules. Sans point : vide.
std::string extension(const std::string &nom) {
    const std::size_t point = nom.find_last_of('.');
    if (point == std::string::npos) {
        return "";
    }
    std::string ext = nom.substr(point + 1);
    for (char &c : ext) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return ext;
}

bool extensionJuste(const std::string &format, const std::string &ext) {
    if (ext.empty()) {
        return false;
    }
    if (format == "JPEG") {
        return ext == "jpg" || ext == "jpeg";
    }
    if (format == "ICO") {
        return ext == "ico" || ext == "cur";
    }
    std::string attendu = format;
    for (char &c : attendu) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return ext == attendu;
}

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n = 0;
    std::cin >> n;

    long long lus = 0;
    long long mensonges = 0;
    long long refuses = 0;

    std::string nom;
    long long taille = 0;
    std::string hex;
    for (int i = 0; i < n && std::cin >> nom >> taille >> hex; ++i) {
        const std::string format = reconnaitre(taille, decoder(hex));
        if (format.empty()) {
            std::cout << nom << " REFUSE\n";
            ++refuses;
            continue;
        }
        ++lus;
        if (extensionJuste(format, extension(nom))) {
            std::cout << nom << ' ' << format << " OK\n";
        } else {
            std::cout << nom << ' ' << format << " MENT\n";
            ++mensonges;
        }
    }

    std::cout << "LUS " << lus << '\n';
    std::cout << "MENSONGES " << mensonges << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
