import Base.==, Base.-, Base.+, Base.<<, Base.>>, Base.*, Base.~
import Base.in, Base.union, Base.intersect, Base.isless, Base.issubset

export BitboardSize, BBS_3, BBS_4, BBS_5, BBS_6, BBS_8
export Direction, DIR_N, DIR_S, DIR_E, DIR_W, DIR_ALL
export Square, slide, coordinates, neighbors
export Bitboard, from_bit_vector, universe, empty, square, row, column

# TODO: i hate this: better to ENUM the directions and have a function compute the deltas
"""
Utility placeholder for the cardinal directions on the board
"""
struct Direction
    dx::Int8
    dy::Int8
end

const DIR_N = Direction(0, 1)
const DIR_S = Direction(0, -1)
const DIR_E = Direction(1, 0)
const DIR_W = Direction(-1, 0)
const DIR_ALL = [DIR_N, DIR_S, DIR_E, DIR_W]

"""
Representation of a square
"""
struct Square
    index::Int8
end

Square(size::BitboardSize, row::Integer, column::Integer) = Square((row - 1) * size + column)

row(size::BitboardSize, sq::Square) = (sq.index - 1) ÷ size + 1
column(size::BitboardSize, sq::Square) = ((c = sq.index % size) == 0 ? size : c)
coordinates(size::BitboardSize, sq::Square) = (row(size, sq), column(size, sq))

# TODO: as below
slide(size::BitboardSize, sq::Square, dir::Direction) = Square(size, row(size, sq) + dir.dy, column(size, sq) + dir.dx)

# TODO: this could be really slow, can i do better?
# maybe using this beauty of a bitboard implementation below?
function neighbors(size::BitboardSize, sq::Square)
    y, x = coordinates(size, sq)
    filter(dir -> 0 < x + dir.dx ≤ size && 0 < y + dir.dy ≤ size, DIR_ALL)
end

"""
Bitboard representation of a board.
"""
struct Bitboard
    size::BitboardSize
    raw::UInt64
end

Bitboard(size::S, raw::R) where {R<:Number,S<:Integer} = Bitboard(BitboardSize(size), UInt64(raw))
Bitboard(size::BitboardSize, sq::Square) = square(BitboardSize(size), sq.index)
from_bit_vector(size::BitboardSize, bits::BitVector) = Bitboard(size, evalpoly(UInt64(2), bits))

in(sq::Square, bb::Bitboard) = Bitboard(bb.size, sq) ⊆ bb
union(b1::Bitboard, b2::Bitboard) = Bitboard(max(b1.size, b2.size), b1.raw | b2.raw)
intersect(b1::Bitboard, b2::Bitboard) = Bitboard(max(b1.size, b2.size), b1.raw & b2.raw)
issubset(b1::Bitboard, b2::Bitboard) = b2 == (b1 ∪ b2)
~(bb::Bitboard) = Bitboard(bb.size, ~bb.raw) ∩ universe(bb.size)
+(b1::Bitboard, b2::Bitboard) = b1 ∪ b2
+(bb::Bitboard, value::Integer) = Bitboard(bb.size, bb.raw + value)
-(b1::Bitboard, b2::Bitboard) = b1 ∩ ~b2
<<(bb::Bitboard, value::Integer) = Bitboard(bb.size, bb.raw << value)
>>(bb::Bitboard, value::Integer) = Bitboard(bb.size, bb.raw >> value)

empty(size::BitboardSize) = Bitboard(size, 0)
universe(size::BitboardSize) = Bitboard(size, UInt64(2)^size^2 - 1)
square(size::BitboardSize, index::Integer) = Bitboard(size, 1) << (index - 1)
row(size::BitboardSize, index::Integer) = Bitboard(size, 2^size - 1) << ((index - 1) * size)
column(size::BitboardSize, index::Integer) = reduce(∪, square(size, r * size) >> (size - index) for r ∈ 1:size)

#https://docs.julialang.org/en/v1/manual/interfaces/#man-interfaces-broadcasting
Base.broadcastable(x::Bitboard) = Ref(Bitboard)
Base.length(bb::Bitboard) = count_ones(bb.raw)

function Base.iterate(bb::Bitboard, state=1)
    if state > bb.size^2
        return nothing
    end
    for index in state:bb.size^2
        s = Square(index)
        if s ∈ bb
            return (s, index + 1)
        end
    end
end

function Base.show(io::IO, bb::Bitboard)
    println("Bitboard{$(bb.size)x$(bb.size)}(0x$(string(bb.raw, base=16, pad=16))):")
    for r ∈ 1:bb.size
        for c ∈ 1:bb.size
            index = (r - 1) * bb.size + c
            print(io, (square(bb.size, index) ⊆ bb) ? "# " : "- ")
        end
        if r < bb.size
            println(io)
        end
    end
end