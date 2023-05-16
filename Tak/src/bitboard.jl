import Base.union, Base.intersect, Base.<<, Base.>>, Base.+, Base.-, Base.~, Base.issubset, Base.in, Base.==

export Bitboard, square, row, column, universe, empty, fromBitVector

struct Bitboard
    raw::UInt64
    size::UInt8
end

union(b1::Bitboard, b2::Bitboard) = Bitboard(b1.raw | b2.raw, max(b1.size, b2.size))
intersect(b1::Bitboard, b2::Bitboard) = Bitboard(b1.raw & b2.raw, max(b1.size, b2.size))
issubset(b1::Bitboard, b2::Bitboard) = b2 == (b1 ∪ b2)
in(b1::Bitboard, b2::Bitboard) = b1 ⊆ b2
~(bb::Bitboard) = Bitboard(~bb.raw, bb.size) ∩ universe(bb.size)
+(b1::Bitboard, b2::Bitboard) = b1 ∪ b2
+(bb::Bitboard, value::Int) = Bitboard(bb.raw + value, bb.size)
-(b1::Bitboard, b2::Bitboard) = b1 ∩ ~b2
-(bb::Bitboard, value::Int) = Bitboard(bb.raw - value, bb.size)
<<(bb::Bitboard, value::Int) = Bitboard(bb.raw << value, bb.size)
>>(bb::Bitboard, value::Int) = Bitboard(bb.raw >> value, bb.size)
==(b1::Bitboard, b2::Bitboard) = b1.raw == b2.raw

fromBitVector(bits::BitVector, size=8) = Bitboard(evalpoly(UInt64(2), bits), size)
row(index::Int, size=8) = Bitboard(2^size - 1, size) << ((index - 1) * size)
column(index::Int, size=8) = reduce(∪, [square(r * 8, size) >> (8 - index) for r ∈ range(1, 8)])
square(index::Int, size=8) = Bitboard(1, size) << (index - 1)
universe(size=8) = Bitboard(UInt64(2)^(size * size) - 1, size)
empty(size=8) = Bitboard(0, size)

#https://docs.julialang.org/en/v1/manual/interfaces/#man-interfaces-broadcasting
Base.broadcastable(x::Bitboard) = Ref(Bitboard)

function Base.iterate(bb::Bitboard, state=1)
    if state > bb.size^2
        return nothing
    end

    for index in state:bb.size^2
        s = square(index, bb.size)
        if s ∈ bb return (s, index + 1) end
    end
end

Base.eltype(::Type{Bitboard}) = Bitboard
Base.length(bb::Bitboard) = count_ones(bb.raw)

function Base.show(io::IO, bb::Bitboard)
    println("Bitboard{$(bb.size)x$(bb.size)}(0x$(string(bb.raw, base=16, pad=16))):")
    for r ∈ range(1, bb.size)
        for c ∈ range(1, bb.size)
            index = (r - 1) * bb.size + c
            print(io, (square(index) ∈ bb) ? "# " : "- ")
        end
        if r < bb.size println(io) end
    end
end