export move, perft

"""
Generate all possible moves in a given position
"""
moves(pos::Position) = pos.move ≥ 2 ? [placements(pos); slides(pos);] : placements(pos)

function perft(pos::Position, depth::Int)
    depth == 0 && return 1

    amoves = moves(pos)
    depth == 1 && return length(amoves)

    nodes = 0
    for move ∈ amoves
        npos = deepcopy(pos)
        nodes += perft(apply!(npos, move), depth - 1)
    end
    nodes
end