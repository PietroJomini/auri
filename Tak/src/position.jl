using StaticArrays

export Position, fromArrays

struct Position
    size::UInt8
    white::Bitboard
    black::Bitboard
    caps::Bitboard
    walls::Bitboard
    heights::SVector{64,UInt8}
    stacks::SVector{64,Bitboard}
end

function fromArrays(size::Int, stacks::Vector{Vector{Int}}, caps::Vector{Int}, walls::Vector{Int})::Position
    p_heights = [0 for _ in range(1, 64)]
    p_stacks = [empty() for _ in range(1, 64)]

    for (index, stack) ∈ pairs(stacks)
        for piece in stack
            p_heights[index] += 1
            p_stacks[index] = (p_stacks[index] << 1) + (piece - 1)
        end
    end

    ceiling = map(stack -> length(stack) > 0 ? stack[end] : 0, stacks)
    white = fromBitVector(ceiling .== 1, size)
    black = fromBitVector(ceiling .== 2, size)
    caps = reduce(∪, square.(caps, size), init=square(0, size))
    walls = reduce(∪, square.(walls, size), init=square(0, size))
    heights = SVector{64}(p_heights)
    stacks = SVector{64}(p_stacks)

    Position(size, white, black, caps, walls, heights, stacks)
end

function Base.show(io::IO, position::Position)
    print(io, "Position\
        {$(position.size)x$(position.size)}\
        [\"$(Tak.TPS.position2tps(position))\"]")
end