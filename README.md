<p align="center">
  <img src="docs/resources/auri.webp" alt="auri" height="236"/>
</p>

<h1 align="center">auri</h1>
<p align="center"><i>“She knew the true shape of the world. All else was shadow and the sound of distant drums.”</i></p>

<br>

## tei interface

`/tei` exposes a minimalistic `tei` interface, that can be compiled with `make tei` (resulting in the bin `/bin/tei`). Supported commands are:

| option   | arguments                    | description                                                                                                                                                                                   |
| -------- | ---------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `help`   |                              |                                                                                                                                                                                               |
| `quit`   |                              | quit the io loop                                                                                                                                                                              |
| `print`  | `[tps\|pretty]`              | if `tps` print the position encoded as a [tps](https://ustak.org/tak-positional-system-tps/) string.<br>if `pretty`  or no arguments pretty-print the whole position.                         |
| `new`    | `<size>`                     | set the current position as a new position of size `size`                                                                                                                                     |
| `tps`    | `<tps>`                      | parse the given tps string and into the current position                                                                                                                                      |
| `move`   | `[<move 1> ... <move n>]`    | apply a series of [ptn](https://ustak.org/portable-tak-notation/)-encoded moves at the current position                                                                                       |
| `search` | `[spread\|placement\|all]`   | search legal moves in the current position. <br> no arguments defaults to `all`                                                                                                               |
| `perft`  | `<depth>`                    | [perft](https://www.chessprogramming.org/Perft)                                                                                                                                               |
| `perftd` | `<depth>`                    | [divide perft](https://www.chessprogramming.org/Perft#Divide)                                                                                                                                 |
| `random` | `[-n <N>] [-m <N>] [-M <N>]` | generate random non-terminated positions, starting from the current one<br>`-n <N>`: amount of position to generate<br>`-m <N>`: min randomization depth<br>`-M <N>`: max randomization depth |

### script-like usage

`tei` now replace the old `scripts`, as it can be used like
```bash
echo "random -n 10" | tei
tei <<< "move a1 b2" <<< "print tps"
```

## taklib

`taklib`, located in `/lib`, is meant as a fast (TODO: bench) header-only `c` library for handling a tak game, used as a base for the other tools implemented in `auri`. Implemented features are:
- [x] bitboard position representation
- [x] moves (placements and spreads) generation and application
- [x] zobrist hashing + transposition table
- [x] tei interface + some commands

For more informations regarding the positon encoding and the moves generation strategy see [the docs](/docs/encoding.md).

### examples

#### importing `taklib`
`taklib` is a set of header-only c files. If you are confused by this definition, [this](https://github.com/nothings/stb#how-do-i-use-these-libraries) should help. Altough kinda controversial, this choice makes a lot of things easier, and as a user the only difference is that, before importing, it's necesary to define the `TAKLIB_IMPLEMENTATION` macro:
```c
// first we define the macro
#define TAKLIB_IMPLEMENTATION

// then we include the headers we need
#include "tak.h"
#include "ptn.h"
```

#### creating an empty position

a `position` is the representation of a state of the board, like a "screenshot" of a moment in a game where a move has just been made.

```c
// this creates a new, empty, position
tak_position pos = tak_newposition(5);
```

There are two lookup tables that are required to run most of the functionalities of the library: `tak_slt` and `tak_zobrist_data`. The first contains all the possible [spread configurations](/docs/encoding.md#slides) and is used to search for spreads, while the second one contains the randomly generated bitstrings needed fot [zobrist hashing](https://en.wikipedia.org/wiki/Zobrist_hashing).

```c
tak_slt slt = tak_slt_load();
tak_zobrist_data zd = tak_zd_load();
```

#### moves

A tak `move` is handled by `taklib` as a combination of anonymous unions and structs that allows to have the same struct (`tak_move`) represent both a placement and a [spread](/docs/encoding.md#packing), and the field `tak_move.type` is set to `TAK_SPREAD` of `TAK_PLACEMENT` to indicate it's type.

Moves can be created manually, although not so convenient, or can be searched for

```c
// create a buffer to store found moves
tak_move buffer[TAK_MAX_MOVES];

// search all legal moves in a given position
int n = tak_search(buffer, &pos, &slt); // returns the amount of 
                                        // moves found
```

Spreads and placements can also be searched individually with `tak_search_spreads` and `tak_search_placements`.

> [!NOTE]
> By default `taklib` assumes swap opening. If we desire to remove this, we can set the `TAKLIB_SWAP` macro to `0` (or a different number to modify the number of plies for which the rule apply, default `2`) both in code
> ```c
> #define TAKLIB_SWAP 0
> #include "tak.h"  // no swap now
> ```
> or at compilation time with `-DTAKLIB_SWAP=0` (on GCC).

To apply a move simply

```c
tak_position before = tan_newposition(5);
tak_position after = tak_do(before, a_move, &zd);
```

and the type-specifics functions are `tak_do_spread` and `tak_do_placement`. As the `tak_do*` functions copy the "before" position insthead of modifying it no `tak_undo*` is required. This proved to be faster with optimized GCC builds than passing references and undoing.

#### handling `tps` strings

`tps` string are handled using the `ptn.h` module. We can parse a string into a position

```c
char tps_string[] = "x3,12,2S/x,22S,22C,11,21/121,212,12,1121C,1212S/21S,1,21,211S,12S/x,21S,2,x2 1 26";
tak_position pos = tps_parse(tps_string, TPS_STD, &zd);
```

The `&zd` parameter is a reference to the zobrist lookup table we made earlier, while `TPS_STD` is the standard tps lexicon. A `lexicon`, both in `tps` and `ptn`, is a set of declarations that defines the characters used to represent stuff in the strings.

We can also encode a previously created position as a new tps string
```c
// create a new position, loading it from a tps string
tak_position pos = tps_parse("x3/x3/x3 1 1");

// and encode it into a new string
char tps[TPS_MAX_LENGTH];
int n = tps_encode(tps, pos, TPS_STD); // returns the amout of charachters
                                       // written into `tps, included '\0'

assert(strcmp(tps, "x3/x3/x3 1 1") == 0) // true
```

> [!WARNING]
> Loading both `tps` and `ptn` strings does not imply a legality check (yet), so trying to load a faulty string can lead to a variety of side effects, ranging from strangely-formed position to segfaults.

#### handling `ptn` moves

`ptn`-encoded moves can be handled in a similar manner as `tps`strings

```c
// load
tak_move move = ptn_parse("4d2>112*");

// and encode
char ptn[PTN_MAX_MOVE_LENGTH];
int n = ptn_encode(ptn, move, /*the size of the board*/ 5, PTN_STD),
```

Note that loading a ptn move is unrelated from any position, so before applying a loaded move to a position without assertions may be risky.

There is a utility function that allows to parse a position from a list of ptn moves

```c
char *moves[] = {"a1", "a2", "b1<"};
tak_position p = ptn_apply(tak_newposition(3), moves, 3, PTN_STD, &zd);

// -> p = 21,x/x3/x3 2 2
```