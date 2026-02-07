#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>

namespace {

struct GameState {
  int lane = 1;
  int lives = 3;
  int nubixLocal = 0;
  int nubixNetwork = 0;
  int score = 0;
};

struct NetworkConfig {
  int listenPort = 0;
  std::string peerIp;
  int peerPort = 0;
  bool enabled = false;
};

class PeerCoinSync {
 public:
  explicit PeerCoinSync(const NetworkConfig& config) : config_(config) {
    if (!config_.enabled) {
      return;
    }

    socketFd_ = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd_ < 0) {
      std::cerr << "[net] Impossibile creare socket UDP, modalità offline.\n";
      enabled_ = false;
      return;
    }

    sockaddr_in localAddr{};
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(static_cast<uint16_t>(config_.listenPort));

    if (::bind(socketFd_, reinterpret_cast<sockaddr*>(&localAddr), sizeof(localAddr)) < 0) {
      std::cerr << "[net] Bind fallita sulla porta " << config_.listenPort << ", modalità offline.\n";
      ::close(socketFd_);
      socketFd_ = -1;
      enabled_ = false;
      return;
    }

    peerAddr_.sin_family = AF_INET;
    peerAddr_.sin_port = htons(static_cast<uint16_t>(config_.peerPort));
    if (::inet_pton(AF_INET, config_.peerIp.c_str(), &peerAddr_.sin_addr) != 1) {
      std::cerr << "[net] IP peer non valido, modalità offline.\n";
      ::close(socketFd_);
      socketFd_ = -1;
      enabled_ = false;
      return;
    }

    enabled_ = true;
    std::cout << "[net] Peer-to-peer attivo: listen=" << config_.listenPort << " peer=" << config_.peerIp << ':'
              << config_.peerPort << "\n";
  }

  ~PeerCoinSync() {
    if (socketFd_ >= 0) {
      ::close(socketFd_);
    }
  }

  bool enabled() const { return enabled_; }

  void broadcastCoinPickup() {
    if (!enabled_) {
      return;
    }

    constexpr char kMessage[] = "COIN:1";
    ::sendto(socketFd_, kMessage, sizeof(kMessage) - 1, 0, reinterpret_cast<const sockaddr*>(&peerAddr_),
             sizeof(peerAddr_));
  }

  int pollRemoteCoins() {
    if (!enabled_) {
      return 0;
    }

    int total = 0;
    for (;;) {
      char buffer[64];
      sockaddr_in from{};
      socklen_t fromLen = sizeof(from);

      const int flags = MSG_DONTWAIT;
      const auto n = ::recvfrom(socketFd_, buffer, sizeof(buffer) - 1, flags, reinterpret_cast<sockaddr*>(&from),
                                &fromLen);
      if (n <= 0) {
        break;
      }

      buffer[n] = '\0';
      if (std::strncmp(buffer, "COIN:", 5) == 0) {
        total += std::atoi(buffer + 5);
      }
    }

    return total;
  }

 private:
  NetworkConfig config_;
  int socketFd_ = -1;
  bool enabled_ = false;
  sockaddr_in peerAddr_{};
};

std::optional<NetworkConfig> parseNetworkConfig(int argc, char** argv) {
  NetworkConfig config;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--listen" && i + 1 < argc) {
      config.listenPort = std::stoi(argv[++i]);
    } else if (arg == "--peer-ip" && i + 1 < argc) {
      config.peerIp = argv[++i];
    } else if (arg == "--peer-port" && i + 1 < argc) {
      config.peerPort = std::stoi(argv[++i]);
    } else if (arg == "--help") {
      return std::nullopt;
    }
  }

  if (config.listenPort > 0 && !config.peerIp.empty() && config.peerPort > 0) {
    config.enabled = true;
  }

  return config;
}

int randomLane(std::mt19937& gen) {
  std::uniform_int_distribution<int> dist(0, 2);
  return dist(gen);
}

void printHud(const GameState& state, bool netEnabled) {
  std::cout << "\n=== NUBIALOX: Corsa Nubix (C++) ===\n";
  std::cout << "Score: " << state.score << " | Vite: " << state.lives << "\n";
  std::cout << "Nubix locali: " << state.nubixLocal << " | Nubix peer: " << state.nubixNetwork
            << " | Totale: " << (state.nubixLocal + state.nubixNetwork) << "\n";
  std::cout << "Lane attuale: " << state.lane << " (0 sinistra, 1 centro, 2 destra)\n";
  std::cout << "Modalità rete: " << (netEnabled ? "P2P attiva" : "Offline") << "\n";
}

void printFrame(int obstacleLane, int coinLane, const GameState& state) {
  for (int lane = 0; lane < 3; ++lane) {
    std::string row = "[ ]";
    if (lane == state.lane) {
      row = "[P]";
    }
    if (lane == obstacleLane) {
      row += "  X";
    }
    if (lane == coinLane) {
      row += "  O";
    }
    std::cout << "Lane " << lane << " " << row << "\n";
  }
}

bool playTurn(GameState& state, std::mt19937& gen, PeerCoinSync& sync) {
  state.nubixNetwork += sync.pollRemoteCoins();

  const int obstacleLane = randomLane(gen);
  const int coinLane = randomLane(gen);

  printHud(state, sync.enabled());
  printFrame(obstacleLane, coinLane, state);

  std::cout << "Comando [a=sinistra, d=destra, invio=resta, q=esci]: ";
  std::string input;
  std::getline(std::cin, input);

  if (!std::cin.good()) {
    return false;
  }

  if (!input.empty()) {
    if (input[0] == 'q' || input[0] == 'Q') {
      return false;
    }
    if ((input[0] == 'a' || input[0] == 'A') && state.lane > 0) {
      --state.lane;
    }
    if ((input[0] == 'd' || input[0] == 'D') && state.lane < 2) {
      ++state.lane;
    }
  }

  if (state.lane == obstacleLane) {
    --state.lives;
    std::cout << "Hai colpito un glitch! -1 vita.\n";
  }

  if (state.lane == coinLane) {
    ++state.nubixLocal;
    state.score += 25;
    std::cout << "Hai raccolto un Nubix!\n";
    sync.broadcastCoinPickup();
  }

  state.score += 10;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  return state.lives > 0;
}

void printHelp() {
  std::cout << "Uso:\n"
            << "  ./nubialox_game [--listen <porta>] [--peer-ip <ip>] [--peer-port <porta>]\n\n"
            << "Esempio P2P (2 terminali):\n"
            << "  Terminale A: ./nubialox_game --listen 9001 --peer-ip 127.0.0.1 --peer-port 9002\n"
            << "  Terminale B: ./nubialox_game --listen 9002 --peer-ip 127.0.0.1 --peer-port 9001\n";
}

}  // namespace

int main(int argc, char** argv) {
  const auto cfg = parseNetworkConfig(argc, argv);
  if (!cfg.has_value()) {
    printHelp();
    return 0;
  }

  std::cout << "Nubialox mini-game in C++ (versione terminale).\n";

  std::random_device rd;
  std::mt19937 gen(rd());

  PeerCoinSync sync(cfg.value());
  GameState state;
  while (playTurn(state, gen, sync)) {
  }

  std::cout << "\nPartita terminata. Score finale: " << state.score << ", Nubix locali: " << state.nubixLocal
            << ", Nubix peer: " << state.nubixNetwork << "\n";
  return 0;
}
