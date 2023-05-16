import Base.==, Base.-, Base.+, Base.<<, Base.>>, Base.*
import Base.union, Base.intersect, Base.isless, Base.issubset

export BitboardSize, BBS_3, BBS_4, BBS_5, BBS_6, BBS_8
export Bitboard, from_bit_vector, universe, empty, square, row, column

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
Bitboard representation of a board.
"""
struct Bitboard
    raw::UInt64
    size::BitboardSize
end

Bitboard(raw::R, size::S) where {R<:Number,S<:Integer} = Bitboard(UInt64(raw), BitboardSize(size))
from_bit_vector(size::BitboardSize, bits::BitVector) = Bitboard(evalpoly(UInt64(2), bits), size)

union(b1::Bitboard, b2::Bitboard) = Bitboard(b1.raw | b2.raw, max(b1.size, b2.size))
intersect(b1::Bitboard, b2::Bitboard) = Bitboard(b1.raw & b2.raw, max(b1.size, b2.size))
issubset(b1::Bitboard, b2::Bitboard) = b2 == (b1 ∪ b2)
~(bb::Bitboard) = Bitboard(~bb.raw, bb.size) ∩ universe(bb.size)
+(b1::Bitboard, b2::Bitboard) = b1 ∪ b2
+(bb::Bitboard, value::Integer) = Bitboard(bb.raw + value, bb.size)
-(b1::Bitboard, b2::Bitboard) = b1 ∩ ~b2
<<(bb::Bitboard, value::Integer) = Bitboard(bb.raw << value, bb.size)
>>(bb::Bitboard, value::Integer) = Bitboard(bb.raw >> value, bb.size)

empty(size::BitboardSize) = Bitboard(0, size)
universe(size::BitboardSize) = Bitboard(UInt64(2)^size^2 - 1, size)
square(size::BitboardSize, index::Integer) = Bitboard(1, size) << (index - 1)
row(size::BitboardSize, index::Integer) = Bitboard(2^size - 1, size) << ((index - 1) * size)
column(size::BitboardSize, index::Integer) = reduce(∪, square(size, r * size) >> (size - index) for r ∈ 1:size)

#https://docs.julialang.org/en/v1/manual/interfaces/#man-interfaces-broadcasting
Base.broadcastable(x::Bitboard) = Ref(Bitboard)

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