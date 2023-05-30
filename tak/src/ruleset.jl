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
const BS3::Size = Size(3)
const BS4::Size = Size(4)
const BS5::Size = Size(5)
const BS6::Size = Size(6)
const BS7::Size = Size(7)
const BS8::Size = Size(8)

"""
Cardinal directions
"""
struct Direction
    dc::Int8
    dr::Int8
end

const DIR_N::Direction = Direction(0, 1)
const DIR_S::Direction = Direction(0, -1)
const DIR_E::Direction = Direction(1, 0)
const DIR_W::Direction = Direction(-1, 0)
const DIR_ALL::Vector{Direction} = [DIR_N, DIR_S, DIR_E, DIR_W]

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
const SETUP::Dict{Size,Tuple{Int, Int}} = Dict(
    BS3 => (10, 0),
    BS4 => (15, 0),
    BS5 => (21, 1),
    BS6 => (30, 1),
    BS7 => (40, 2),
    BS8 => (50, 2)
)