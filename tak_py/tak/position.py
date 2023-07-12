from enum import Flag, auto
from typing import List

class Piece(Flag):
    WHITE = auto()
    BLACK = auto()
    CAP = auto()
    WALL = auto()
    FLAT = auto()

PLAYERS = Piece.WHITE | Piece.BLACK
MODIFIERS = Piece.CAP | Piece.WALL
PIECES = Piece.CAP | Piece.WALL | Piece.FLAT

class Position:
    """store a board setup"""

    def __init__(self, size: int = 5):
        self.stacks: List[List[Piece]] = [[] for _ in range(size * size)]
        self.size = size
        self.move = 0