import Base.==, Base.-, Base.+, Base.<<, Base.>>, Base.*, Base.~
import Base.in, Base.union, Base.intersect, Base.isless, Base.issubset

export BitboardSize, BBS_3, BBS_4, BBS_5, BBS_6, BBS_8
export Square, Bitboard, from_bit_vector, universe, empty, square, row, column

"""
Utility placeholder fot the size of a Bitboard.
"""
BitboardSize = Int8

const BBS_3 = BitboardSize(3)
const BBS_4 = BitboardSize(4)
const BBS_5 = BitboardSize(5)
const BBS_6 = BitboardSize(6)
const BBS_8 = BitboardSize(8)

"""
Representation of a square
"""
struct Square
    index::Int8
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
    if state > bb.size^2 return nothing end
    for index in state:bb.size^2
        s = square(bb.size, index)
        if s ⊆ bb return (s, index + 1) end
    end
end

function Base.show(io::IO, bb::Bitboard)
    println("Bitboard{$(bb.size)x$(bb.size)}(0x$(string(bb.raw, base=16, pad=16))):")
    for r ∈ 1:bb.size
        for c ∈ 1:bb.size
            index = (r - 1) * bb.size + c
            print(io, (square(bb.size, index) ⊆ bb) ? "# " : "- ")
        end
        if r < bb.size println(io) end
    end
end