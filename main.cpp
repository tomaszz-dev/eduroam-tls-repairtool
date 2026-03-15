#include <iostream>
#include <string>
#include <vector>

int main() {
    FILE* pipe = popen("nmcli -t -f NAME connection show", "r");

    std::vector<std::string> connections;
    char buffer[256];
    int prawdopodobne_pol = -1;
    int licznik = 0;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        line.erase(line.find_last_not_of("\n") + 1);
        connections.push_back(line);
        if (line == "eduroam") {
            prawdopodobne_pol = licznik;
        }
        licznik++;
    }
    pclose(pipe);

    std::cout << "Dostępne połączenia:" << std::endl;
    for (int i = 0; i < connections.size(); i++) {
        std::cout << i + 1 << ". " << connections[i] << std::endl;
    }

    std::string wybrane;
    std::string potwierdzenie;

    if (prawdopodobne_pol != -1) {
        std::cout << "Znaleziono połączenie 'eduroam' o numerze " << prawdopodobne_pol + 1 << std::endl;
        std::cout << "Kontynuować? t/n?" << std::endl;
        std::cin >> potwierdzenie;
        if (potwierdzenie == "T" || potwierdzenie == "t") {
            wybrane = connections[prawdopodobne_pol];
        } else {
            std::cout << "Wybierz numer: ";
            int wybor;
            std::cin >> wybor;
            wybrane = connections[wybor - 1];
        }
    } else {
        std::cout << "Wybierz numer: ";
        int wybor;
        std::cin >> wybor;
        wybrane = connections[wybor - 1];
    }

    std::cout << "Wybrano: " << wybrane << std::endl;
    std::string cmd = "nmcli connection modify '" + wybrane + "' 802-1x.phase1-auth-flags 32";
    FILE* pipe2 = popen(cmd.c_str(), "r");
    pclose(pipe2);

    std::cout << "Restartowanie NetworkManager..." << std::endl;
    system("systemctl restart NetworkManager.service");

    std::cout << "Gotowe!" << std::endl;

    return 0;
}