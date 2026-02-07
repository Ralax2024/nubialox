#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>

namespace {

struct GameState {
  int lane = 1;
  int lives = 3;
  int nubix = 0;
  int score = 0;
};

int randomLane(std::mt19937& gen) {
  std::uniform_int_distribution<int> dist(0, 2);
  return dist(gen);
}

void printHud(const GameState& state) {
  std::cout << "\n=== NUBIALOX: Corsa Nubix (C++) ===\n";
  std::cout << "Score: " << state.score << " | Nubix: " << state.nubix << " | Lives: " << state.lives << "\n";
  std::cout << "Lane attuale: " << state.lane << " (0 sinistra, 1 centro, 2 destra)\n";
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

bool playTurn(GameState& state, std::mt19937& gen) {
  int obstacleLane = randomLane(gen);
  int coinLane = randomLane(gen);

  printHud(state);
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
    ++state.nubix;
    state.score += 25;
    std::cout << "Hai raccolto un Nubix!\n";
  }

  state.score += 10;
  std::this_thread::sleep_for(std::chrono::milliseconds(120));
  return state.lives > 0;
}

}  // namespace

int main() {
  std::cout << "Nubialox mini-game in C++ (versione terminale).\n";

  std::random_device rd;
  std::mt19937 gen(rd());

  GameState state;
  while (playTurn(state, gen)) {
  }

  std::cout << "\nPartita terminata. Score finale: " << state.score << ", Nubix: " << state.nubix
            << ", Lives: " << state.lives << "\n";
  return 0;
}
