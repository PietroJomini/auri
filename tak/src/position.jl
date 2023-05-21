using StaticArrays

export Position, startposition

mutable struct Position
    size::BitboardSize
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
        [\"$(Tak.PTN.tps(position))\"]")
end

# TODO: bench against from_fen
startposition(size::BitboardSize) = Position(
    size,
    empty(size),
    empty(size),
    empty(size),
    empty(size),
    SVector{64}(zeros(UInt8, 64)),
    SVector{64}([empty(size) for _ ∈ 1:64])
)