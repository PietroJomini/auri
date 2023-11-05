## Features (aka. todos)
- [ ] engine
  - [x] board representation
  - [ ] full game entity
    - [ ] history
    - [ ] undo moves
    - [ ] variations
    - [ ] comments
  - [ ] moves
    - [x] generation
      - [x] slides
      - [x] placements
    - [x] do
      - [x] slides
      - [x] placements
    - [ ] undo
      - [ ] slides
      - [ ] placements
    - [x] perft
    - [x] swap rule
  - [ ] game endings
    - [x] routes
    - [x] ties
      - [x] no more pieces
      - [ ] full board
    - [x] dragon clause
  - [ ] notations
    - [ ] tps
      - [x] from
      - [x] to
      - [ ] check
    - [ ] ptn
      - [x] moves to, from
      - [ ] game to, from
    - [x] playtak db
- [ ] opening db
- [ ] engine
  - [ ] search
  - [ ] evaluation
    - [ ] hand-made euristic
    - [ ] alpha0
- [ ] engines tournament
- [ ] interfaces
  - [ ] cli
  - [ ] channel to ptn.ninja
    - [ ] fork ptn.ninja to work with tei
  - [ ] tei
- [ ] perft analysis
  - [x] scripts for engines
    - [x] auri
    - [x] topaz
    - [x] tiltak
  - [x] load games from the playtak db
  - [ ] results analysis

## Todos
- [ ] benchmark a undo function vs using passing by value to clone the positin in perft