<p align="center">
  <img src="docs/resources/auri.webp" alt="auri" height="236"/>
</p>

<h1 align="center">auri</h1>
<p align="center"><i>“She knew the true shape of the world. All else was shadow and the sound of distant drums.”</i></p>

<br>

## tei interface

`auri` (accessible by compiling the file `auri.c`) exposes a minimalistic `tei` interface. Supported commands are:
- [x] `help`
- [x] `quit`
- [x] `print`: pretty print the position
- [x] `tps <tps>`: load a tps string
- [x] `new <size>`: load an empty position of given size
- [x] `move <...moves>`: apply moves to the position
- [x] `perft <depth>`: [perft](https://www.chessprogramming.org/Perft)
- [x] `perftd <depth>`: [divide perft](https://www.chessprogramming.org/Perft#Divide)

## taklib

`taklib`, located in `/lib`, is meant as a fast (TODO: bench) header-only `c` library for handling a tak game. Implemented features are:
- [x] bitboard position representation
- [x] moves (placements and spreads) generation and application
- [x] zobrist hashing + transposition table
- [x] tei interface + some commands

For more informations regarding the positon encoding and the moves generation strategy see [the docs](/docs/encoding.md).

### examples

TODO