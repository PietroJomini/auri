export Square, row, column, coordinates, index, ok, slide, neighbors, in

"""
Representation of a square. The value is a packed row-column coordinate.
"""
struct Square
    value::UInt8
end

Square(row::Integer, column::Integer) = Square(UInt8(row) << 4 | column)
Square(index::Integer, size::Size) = Square((index - 1) ÷ size + 1, (c = index % size) == 0 ? Int(size) : c)

row(sq::Square) = sq.value >> 4
column(sq::Square) = sq.value & 0xf
coordinates(sq::Square) = (row(sq), column(sq))
index(sq::Square, size::Size) = (row(sq) - 1) * size + column(sq)

ok(sq::Square, size::Size) = 0 < row(sq) ≤ size && 0 < column(sq) ≤ size
slide(sq::Square, dir::Direction, length::Integer=1) = Square(Int(row(sq) + dir.dr * length), Int(column(sq) + dir.dc * length))
neighbors(sq::Square, size::Size) = filter(s -> ok(s[2], size), map(d -> (d, slide(sq, d)), DIR_ALL))

in(sq::Square, bb::Bitboard, size::Size) = square(index(sq, size)) ⊆ bb