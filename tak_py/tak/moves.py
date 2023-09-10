from dataclasses import dataclass
from itertools import chain
from typing import List

from tak.position import Piece, Position, PLAYERS, SETUP, PIECES
from tak.utils import Direction


@dataclass
class Placement:
    index: int
    kind: Piece
    color: Piece

    def apply(self, p: Position) -> Position:
        p.stacks[self.index].append(self.kind | self.color)
        return p

    def undo(self, p: Position) -> Position:
        p.stacks[self.index].pop()
        return p


def placements(p: Position) -> List[Placement]:
    # compute placement color
    # switch color if it's each player first move
    color = p.turn() if p.move > 1 else PLAYERS ^ p.turn()

    # find empty indexes
    mask = [i for (i, s) in enumerate(p.stacks) if len(s) == 0]

    # if it's each player first move only flats can be placed
    amount = p.count(color)
    kinds = chain(
        [Piece.CAP] if amount[Piece.CAP] < SETUP[p.size][Piece.CAP] else [],
        [Piece.FLAT] if amount[Piece.FLAT] < SETUP[p.size][Piece.FLAT] else [],
        [Piece.WALL] if amount[Piece.FLAT] < SETUP[p.size][Piece.FLAT] else [],
    )

    return [Placement(index, piece, color) for piece in kinds for index in mask]


@dataclass
class Slide:
    origin: int
    direction: Direction
    heights: List[int]
    flattens: bool
