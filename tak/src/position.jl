using StaticArrays, ..BB

export Position, startposition, turn
export Stats, stats

mutable struct Position
    size::Size
    white::Bitboard
    black::Bitboard
    caps::Bitboard
    walls::Bitboard
    heights::MVector{64,UInt8}
    stacks::MVector{64,Bitboard}

    # do I need to move this into a "game" entity?
    move::UInt32
end

Position(tps::String) = Tak.PTN.from_tps(tps)

"""Which player's turn is"""
turn(pos::Position) = Player(rem(pos.move, 2))

function Base.show(io::IO, position::Position)
    print(io, "Position\
        {$(position.size)x$(position.size)}\
        (\"$(Tak.PTN.to_tps(position))\")")
end

startposition(size::Size) = Position(
    size,
    BB.empty(),
    BB.empty(),
    BB.empty(),
    BB.empty(),
    MVector{64}(zeros(UInt8, 64)),
    MVector{64}([BB.empty() for _ ∈ 1:64]),
    0
)

"""
Stats about the number of pieces on the board in the fiven position
"""
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