from enum import Flag, auto
from typing import List, Dict
from itertools import chain


class Piece(Flag):
    WHITE = auto()
    BLACK = auto()
    CAP = auto()
    WALL = auto()
    FLAT = auto()


PLAYERS = Piece.WHITE | Piece.BLACK
MODIFIERS = Piece.CAP | Piece.WALL
PIECES = Piece.CAP | Piece.WALL | Piece.FLAT

SETUP = {
    3: {Piece.FLAT: 10, Piece.CAP: 0},
    4: {Piece.FLAT: 15, Piece.CAP: 0},
    5: {Piece.FLAT: 21, Piece.CAP: 1},
    6: {Piece.FLAT: 30, Piece.CAP: 1},
    7: {Piece.FLAT: 40, Piece.CAP: 2},
    8: {Piece.FLAT: 50, Piece.CAP: 2},
}


class Position:
    """store a board setup"""

    def __init__(self, size: int = 5):
        self.stacks: List[List[Piece]] = [[] for _ in range(size * size)]
        self.size = size
        self.move = 0

    def turn(self) -> Piece.WHITE | Piece.BLACK:
        """which player's turn it is"""

        return Piece.WHITE if self.move % 2 == 0 else Piece.BLACK

    def count(self, color: Piece.WHITE | Piece.BLACK) -> Dict[Piece, int]:
        """count pieces"""

        pieces = chain(*self.stacks)
        pieces = list(filter(lambda p: color in p, pieces))
        caps = len(list(filter(lambda p: Piece.CAP in p, pieces)))

        return {Piece.FLAT: len(pieces) - caps, Piece.CAP: caps}
