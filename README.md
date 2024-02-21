<p align="center">
  <img src="docs/resources/auri.webp" alt="auri" height="236"/>
</p>

<h1 align="center">auri</h1>
<p align="center"><i>“She knew the true shape of the world. All else was shadow and the sound of distant drums.”</i></p>

<br>

## tei interface

`auri` (accessible by compiling the file `auri.c`) exposes a minimalistic `tei` interface. Supported commands are:

| option   | arguments                    | description                                                                                                                                                                                  |
| -------- | ---------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `help`   |                              |                                                                                                                                                                                              |
| `quit`   |                              | quit the io loop                                                                                                                                                                             |
| `print`  | `[tps\|pretty]`              | if `tps` print the position encoded as a [tps]() string.<br>if `pretty`  or no arguments pretty-print the whole position.                                                                    |
| `new`    | `<size>`                     | set the current position as a new position of size `size`                                                                                                                                    |
| `tps`    | `<tps>`                      | parse the given tps string and into the current position                                                                                                                                     |
| `move`   | `[<move 1> ... <move n>]`    | apply a series of [ptn]()-encoded moves at the current position                                                                                                                              |
| `perft`  | `<depth>`                    | [perft](https://www.chessprogramming.org/Perft)                                                                                                                                              |
| `perftd` | `<depth>`                    | [divide perft](https://www.chessprogramming.org/Perft#Divide)                                                                                                                                |
| `random` | `[-n <N>] [-m <N>] [-M <N>]` | generate random non-terminated positions, starting from the current one<br>`-n <N>`: amout of position to generate<br>`-m <N>`: min randomization depth<br>`-M <N>`: max randomization depth |

### script-like usage

`tei` now replace the old `scripts`, as it can be used like
```bash
echo "random -n 10" | auri
auri <<< "move a1 b2" <<< "print tps"
```

## taklib

`taklib`, located in `/lib`, is meant as a fast (TODO: bench) header-only `c` library for handling a tak game. Implemented features are:
- [x] bitboard position representation
- [x] moves (placements and spreads) generation and application
- [x] zobrist hashing + transposition table
- [x] tei interface + some commands

For more informations regarding the positon encoding and the moves generation strategy see [the docs](/docs/encoding.md).

### examples

TODO