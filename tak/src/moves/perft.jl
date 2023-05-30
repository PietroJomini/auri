export perft

function perft(p::Position, depth::Int)
    depth == 0 && return 1
    depth == 1 && return length(moves(p))

    nodes = 0
    for move ∈ moves(p)
        # TODO: undo!(pos, move)
        np = deepcopy(p)
        nodes += perft(apply!(np, move), depth - 1)
    end
    nodes
end