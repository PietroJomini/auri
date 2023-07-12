from dataclasses import dataclass
from typing import Dict

from bidict import bidict

from tak.position import Position, Piece


@dataclass
class Syntax:
    row_separator: str
    square_separator: str
    empty_cell: str
    players: bidict
    pieces: bidict


STD = Syntax(
    row_separator="/",
    square_separator=",",
    empty_cell="x",
    players=bidict({Piece.WHITE: "1", Piece.BLACK: "2"}),
    pieces=bidict({Piece.CAP: "C", Piece.WALL: "S"}),
)


def from_tps(tps: str, syntax: Syntax = STD):
    tps, turn, moves = tps.split(" ")
    p = Position(tps.count(syntax.row_separator) + 1)
    p.move = (int(moves) - 1) * 2 + int(turn) - 1

    for i, row in enumerate(tps.split(syntax.row_separator)):

        # tps starts from the upper row
        # eg. in 5x5, the first cell is a5
        index = (p.size - i - 1) * p.size
        for cell in row.split(syntax.square_separator):

            # jump if x
            if syntax.empty_cell == cell[0]:
                index += int(cell[1]) if len(cell) == 2 else 1
                continue

            # add pieces
            for piece in cell:
                if piece in syntax.players.inverse:
                    p.stacks[index].append(syntax.players.inverse[piece])
            
            # check for caps / walls modifiers
            if cell[-1] in syntax.pieces.inverse:
                p.stacks[index][-1] |= syntax.pieces.inverse[cell[-1]]

            index += 1

    return p
