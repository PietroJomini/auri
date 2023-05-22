"""
A place for all the hard-coded rulebook constants
"""

export Player, White, Black
export Piece, Flatstone, Capstone, Wall
export BitboardSize, BBS_3, BBS_4, BBS_5, BBS_6, BBS_7, BBS_8
export SETUP

"""Players enumeration"""
@enum Player begin
    White = 0
    Black = 1
end

opponent(player::Player) = player == White ? Black : White
symbol(player::Player) = player == White ? :white : :black

"""Pieces kinds enumeration"""
@enum Piece begin
    Flatstone
    Capstone
    Wall
end

"""
Utility placeholder for the size of a Bitboard.
"""
BitboardSize = Int8

const BBS_3 = BitboardSize(3)
const BBS_4 = BitboardSize(4)
const BBS_5 = BitboardSize(5)
const BBS_6 = BitboardSize(6)
const BBS_7 = BitboardSize(7)
const BBS_8 = BitboardSize(8)

"""
Amount of stones per board size
"""
SETUP = Dict(
    BBS_3 => (10, 0),
    BBS_4 => (15, 0),
    BBS_5 => (21, 1),
    BBS_6 => (30, 1),
    BBS_7 => (40, 2),
    BBS_8 => (50, 2)
)