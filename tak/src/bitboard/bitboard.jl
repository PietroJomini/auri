import Base.~, Base.-, Base.+, Base.<<, Base.>>
import Base.union, Base.intersect, Base.issubset

export Bitboard, universe, empty, square, row, column

struct Bitboard 
    value::UInt64
end

Bitboard(value::Integer) = Bitboard(UInt64(value))
Bitboard(bits::BitVector) = Bitboard(evalpoly(UInt64(2), bits))

empty() = Bitboard(0)
universe() = universe(BS8)
universe(size::Size) = universe(Int(size)^2)
universe(size::Integer) = Bitboard(UInt64(2)^size - 1)

union(a::Bitboard, b::Bitboard) = Bitboard(a.value | b.value)
intersect(a::Bitboard, b::Bitboard) = Bitboard(a.value & b.value)
issubset(a::Bitboard, b::Bitboard) = b == (a ∪ b)
~(a::Bitboard) = Bitboard(~a.value)
-(a::Bitboard, b::Bitboard) = a ∩ ~b
+(a::Bitboard, value::Integer) = Bitboard(a.value + value)
<<(a::Bitboard, value::Integer) = Bitboard(a.value << value)
>>(a::Bitboard, value::Integer) = Bitboard(a.value >> value)

# TODO: should I precompile squares, columns and rows?
square(index::Integer) = Bitboard(1) << (index - 1)
row(size::Size, index::Integer) = Bitboard(2^size - 1) << ((index - 1) * size)
column(size::Size, index::Integer) = reduce(∪, square(r * size) >> (size - index) for r ∈ 1:size)

#https://docs.julialang.org/en/v1/manual/interfaces/#man-interfaces-broadcasting
Base.broadcastable(x::Bitboard) = Ref(Bitboard)
Base.length(bb::Bitboard) = count_ones(bb.value)

function Base.iterate(bb::Bitboard, state=1)
    if state > 64
        return nothing
    end
    for index ∈ state:64
        s = square(index)
        if s ⊆ bb
            return (index, index + 1)
        end
    end
end

# IO
function Base.show(io::IO, bb::Bitboard)
    size = Size(get(io, :size, 8))
    println("Bitboard{$(size)x$(size)}(0x$(string(bb.value, base=16, pad=16))):")
    for r ∈ 1:size
        for c ∈ 1:size
            print(io, in(Square(r, c), bb, size) ? "# " : "- ")
        end
        if r < size
            println(io)
        end
    end
end