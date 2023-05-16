# position

```
size    uint8       (req 3 bits)
white   uint64
black   uint64
caps    uint64
walls   uint64
height  uint8 * 64
stacks  uint64 * 64
        ------
        4864 (608 bytes)
```

- overflow error on max stack height on 8x8
- on smaller boards (n * n) use the first n * n needed bits

# move

```
placement       uint64
movement
    origin      uint64
    direction   uint8           (req 2 bits)
    length      uint8           (req 3 bits)
    height      uint8 * length  (max * 8)
                ------
                max 144 (18 bytes)
```

- combine direction and length

# tps

- i can infer the size of the board from the number of '/', so i can extend the notation to allow (eg. on a 5*5 board) "x5/" to be collapsed in "/"
  - "11,2,x5/..." -> "11,2/..."
  - "x5/x5/x5/x5/x5" -> "////"
- should i ignore random occurring stuff of throw?