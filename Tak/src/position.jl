using StaticArrays

export Position, fromArrays

struct Position
    size::Int8
    white::Bitboard
    black::Bitboard
    caps::Bitboard
    walls::Bitboard
    heights::SVector{64,UInt8}
    stacks::SVector{64,Bitboard}
end

function Base.show(io::IO, position::Position)
    print(io, "Position\
        {$(position.size)x$(position.size)}\
        [\"$(Tak.TPS.tps(position))\"]")
end