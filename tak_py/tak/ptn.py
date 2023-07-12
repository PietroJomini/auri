from dataclasses import dataclass
from typing import Dict

from bidict import bidict

from tak.position import Position, Piece, MODIFIERS, PLAYERS


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


def from_tps(tps: str, syntax: Syntax = STD) -> Position:
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

def to_tps(p: Position, syntax: Syntax = STD) -> str:
    rows = [[] for _ in range(p.size)]
    jumps = 0

    for row in range(p.size):
        for col in range(p.size):

            # start from the last row
            index = (p.size - row - 1) * p.size + col

            # check if the cell is empty
            if len(p.stacks[index]) == 0:
                jumps += 1
            else:

                # check for jumps
                if jumps > 0:
                    amount = "" if jumps == 1 else str(jumps)
                    rows[row].append(f"{syntax.empty_cell}{amount}")
                    jumps = 0
                
                # add cell
                stack = [piece & PLAYERS for piece in p.stacks[index]]
                cell = "".join([syntax.players[piece] for piece in stack])
                modifier = syntax.pieces[p.stacks[index][-1] & MODIFIERS] if p.stacks[index][-1] & MODIFIERS in syntax.pieces else ""
                rows[row].append(cell + modifier)

    # this is annoying
    if jumps > 0:
        amount = "" if jumps == 1 else str(jumps)
        rows[row].append(f"{syntax.empty_cell}{amount}")
        jumps = 0

    rows = map(syntax.square_separator.join, rows)
    tps = syntax.row_separator.join(rows)
    move, turn = divmod(p.move, 2)
    return f"{tps} {turn + 1} {move + 1}"
