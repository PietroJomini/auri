using StaticArrays
import Base.==

export Position, turn, startposition, random, stats

mutable struct Position
    size::BitboardSize
    white::Bitboard
    black::Bitboard
    caps::Bitboard
    walls::Bitboard
    heights::MVector{64,UInt8}
    stacks::MVector{64,Bitboard}

    # do I need to move this into a "game" entity?
    move::UInt32
end

turn(pos::Position) = Player(rem(pos.move, 2))

==(a::Position, b::Position) = (a.size == b.size && a.white == b.white && a.black == b.black && a.caps == b.caps && a.walls == b.walls && a.heights == b.heights && a.stacks == b.stacks && a.move == b.move)


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
    MVector{64}(zeros(UInt8, 64)),
    MVector{64}([empty(size) for _ ∈ 1:64]),
    0
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

    p.heights = MVector{64}(heights)
    p.stacks = MVector{64}(stacks)
    p
end

struct Stats
    white::Tuple{Int,Int}
    black::Tuple{Int,Int}
end

function stats(pos::Position)
    bcaps = length(pos.caps ∩ pos.black)
    wcaps = length(pos.caps ∩ pos.white)
    bflats = sum(map(stack -> length(stack), pos.stacks)) - bcaps
    wflats = sum(pos.heights) - bflats - length(pos.caps)
    Stats((wflats, wcaps), (bflats, bcaps))
end