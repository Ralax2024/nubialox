# Nubialox (C++)

Nubialox è una piattaforma di gioco e creazione multiplayer che permette agli utenti di giocare, creare e pubblicare esperienze 3D all’interno di un unico ecosistema condiviso.

Ogni esperienza è un mondo indipendente, sviluppato dalla community, con regole, stili e meccaniche proprie.

## Caratteristiche principali

- **Editor intuitivo** per costruire mondi 3D in modo rapido.
- **Sistema di script semplice** per aggiungere logiche e interazioni.
- **Server online integrati** per giocare e testare subito le esperienze.
- **Collaborazione tra creatori** con strumenti pensati per il lavoro condiviso.
- **Compatibilità leggera**: progettato per funzionare anche su hardware non potente.

## Economia: Nubix

La valuta ufficiale di Nubialox è **Nubix**:

- economica;
- ottenibile anche giocando;
- utilizzata solo per contenuti estetici, servizi opzionali e supporto ai creatori.

Nubialox non è pay-to-win e mantiene un’economia trasparente e giusta.

## Mini gioco incluso (solo C++)

Questa repository include una demo terminale in C++ chiamata **"Nubialox: Corsa Nubix"**.

### Build e avvio

```bash
cmake -S . -B build
cmake --build build
./build/nubialox_game
```

## Perché non vedi nulla sul repo GitHub?

Le modifiche fatte qui sono locali finché non vengono pubblicate su remoto. Per inviarle su GitHub devi fare push del branch:

```bash
git push origin <nome-branch>
```
