# Nubialox (C++)

Nubialox è una piattaforma di gioco e creazione multiplayer che permette agli utenti di giocare, creare e pubblicare esperienze 3D all’interno di un unico ecosistema condiviso.

Ogni esperienza è un mondo indipendente, sviluppato dalla community, con regole, stili e meccaniche proprie.

## Mini gioco incluso (solo C++)

Questa repository include una demo terminale in C++ chiamata **"Nubialox: Corsa Nubix"**.

Funzionalità principali:
- corsie con ostacoli (`X`) e monete (`O`);
- punteggio, vite e raccolta Nubix locali;
- sincronizzazione **peer-to-peer UDP** delle monete raccolte tra due istanze.

## Build e avvio

```bash
cmake -S . -B build
cmake --build build
./build/nubialox_game
```

## Avvio peer-to-peer monete

Apri due terminali:

```bash
# Terminale A
./build/nubialox_game --listen 9001 --peer-ip 127.0.0.1 --peer-port 9002

# Terminale B
./build/nubialox_game --listen 9002 --peer-ip 127.0.0.1 --peer-port 9001
```

Quando un peer raccoglie una moneta, l’altro riceve l’aggiornamento nel contatore "Nubix peer".

## Dimensione binario (~200 MB)

Il `CMakeLists.txt` applica un post-build che porta l’eseguibile a circa **200 MB** (`truncate -s 200M`) come richiesto.

## Perché non vedi nulla sul repo GitHub?

Le modifiche fatte qui sono locali finché non vengono pubblicate su remoto. Per inviarle su GitHub devi fare push del branch:

```bash
git push origin <nome-branch>
```
