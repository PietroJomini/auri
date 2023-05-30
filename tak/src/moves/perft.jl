export perft, perftd

function perft(p::Position, depth::Int)
    depth < 1 && return 1
    depth == 1 && return length(moves(p))

    nodes = 0
    for move ∈ moves(p)
        apply!(p, move)
        nodes += perft(p, depth - 1)
        undo!(p, move)
    end
    nodes
end

# https://www.chessprogramming.org/Perft#Divide
function perftd(p::Position, depth::Int)
    nodes = 0
    for move ∈ moves(p)
        # TODO: undo!(pos, move)
        np = deepcopy(p)
        nodes += (pcount = perft(apply!(np, move), depth - 1))
        println("$move: $pcount")
    end
    println("Node searched: $nodes")
end