#include <iostream>
#include <string>

int main() {
    int n = 0;
    std::cin >> n;

    long long points = 0;
    long long segments = 0;
    long long triangles = 0;
    long long refuses = 0;

    for (int i = 0; i < n; ++i) {
        std::string type;
        long long s = 0;
        std::cin >> type >> s;

        std::string unite;
        long long nombre = 0;
        long long restants = 0;

        if (type == "POINTS") {
            unite = "POINTS";
            nombre = s;
        } else if (type == "LINES") {
            unite = "SEGMENTS";
            nombre = s / 2;
            restants = s % 2;
        } else if (type == "LINE_STRIP") {
            unite = "SEGMENTS";
            if (s >= 2) {
                nombre = s - 1;
            } else {
                restants = s;
            }
        } else if (type == "TRIANGLES") {
            unite = "TRIANGLES";
            nombre = s / 3;
            restants = s % 3;
        } else if (type == "TRIANGLE_STRIP" || type == "TRIANGLE_FAN") {
            unite = "TRIANGLES";
            if (s >= 3) {
                nombre = s - 2;
            } else {
                restants = s;
            }
        } else {
            std::cout << type << ' ' << s << " REFUSE\n";
            ++refuses;
            continue;
        }

        std::cout << type << ' ' << s << ' ' << nombre << ' ' << unite << ' ' << restants << '\n';

        if (unite == "POINTS") {
            points += nombre;
        } else if (unite == "SEGMENTS") {
            segments += nombre;
        } else {
            triangles += nombre;
        }
    }

    std::cout << "POINTS " << points << '\n';
    std::cout << "SEGMENTS " << segments << '\n';
    std::cout << "TRIANGLES " << triangles << '\n';
    std::cout << "REFUSES " << refuses << '\n';
    return 0;
}
