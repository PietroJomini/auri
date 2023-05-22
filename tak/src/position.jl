using StaticArrays

export Position, startposition, random

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

# TODO: walls and caps
# TODO: better to simulate a real game
#       using moves? maybe it will be more realistic
function random(size::BitboardSize)
    p = startposition(size)
    heights = [UInt8(0) for _ ∈ 1:64]
    stacks = [empty(size) for _ ∈ 1:64]

    nflats = 2 * rand(1:SETUP[size][1])
    colors = [White, Black]
    for _ ∈ 1:nflats
        index = rand(1:size^2)
        color = rand(colors)

        # place the stone
        csymbol = symbol(color)
        setfield!(p, csymbol, getfield(p, csymbol) ∪ square(size, index))
        stacks[index] = (stacks[index] << 1) + Int(color)
        heights[index] += 1

        # eventually remove color
        length(getfield(p, csymbol)) == SETUP[size][1] && (colors = [opponent(color)])
    end

    p.heights = SVector{64}(heights)
    p.stacks = SVector{64}(stacks)
    p
end