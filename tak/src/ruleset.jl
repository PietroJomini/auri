"""
A place for all the hard-coded rulebook constants 
(and some other stuff that had no other place :=})
"""

import Base.-

export Size, BS3, BS4, BS5, BS6, BS7, BS8
export Direction, DIR_N, DIR_S, DIR_E, DIR_W, DIR_ALL
export Piece, Flatstone, Capstone, Wall, shape
export Player, White, Black, opponent, symbol
export SETUP

"""
Board size.
"""
Size = UInt8
BS3 = Size(3)
BS4 = Size(4)
BS5 = Size(5)
BS6 = Size(6)
BS7 = Size(7)
BS8 = Size(8)

"""
Cardinal directions
"""
struct Direction
    dc::Int8
    dr::Int8
end

const DIR_N = Direction(0, 1)
const DIR_S = Direction(0, -1)
const DIR_E = Direction(1, 0)
const DIR_W = Direction(-1, 0)
const DIR_ALL = [DIR_N, DIR_S, DIR_E, DIR_W]

-(d::Direction) = Direction(-d.dc, -d.dr)

"""
Pieces kinds enumeration
"""
@enum Piece begin
    Flatstone
    Wall
    Capstone
end

shape(p::Piece) = Piece(Int(p) ÷ 2 + 1)

"""Players enumeration"""
@enum Player begin
    White = 0
    Black = 1
end

opponent(p::Player) = p == White ? Black : White
symbol(player::Player) = player == White ? :white : :black

"""
Amount of stones per board size
"""
SETUP = Dict(
    BS3 => (10, 0),
    BS4 => (15, 0),
    BS5 => (21, 1),
    BS6 => (30, 1),
    BS7 => (40, 2),
    BS8 => (50, 2)
)