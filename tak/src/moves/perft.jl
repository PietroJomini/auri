export perft1, perft

struct PerftResult
    total::Int
    nplacements::Int
    nslides::Int
    placements::Vector{Placement}
    slides::Vector{Slide}
end

Base.show(io::IO, pr::PerftResult) = print(io, "PerfResult[$(pr.total); placements=$(pr.nplacements); slides=$(pr.nslides)]")

function perft1(pos::Position, color::Player)
    remp = getfield(stats(pos), symbol(color)) .< SETUP[pos.size]
    pls = placements(pos; flats=remp[1], walls=remp[1], caps=remp[2])

    sls = []
    for p ∈ getfield(pos, symbol(color))
        append!(sls, slides(pos, p))
    end

    npls = length(pls)
    nsls = length(sls)
    PerftResult(npls + nsls, npls, nsls, pls, sls)
end

function perft(pos::Position, color::Player, depth::Int)    
    remp = getfield(stats(pos), symbol(color)) .< SETUP[pos.size]
    pls = placements(pos; flats=remp[1], walls=remp[1], caps=remp[2])

    sls = []
    for p ∈ getfield(pos, symbol(color))
        append!(sls, slides(pos, p))
    end

    # batch parft
    depth == 1 && return length([pls; sls])

    nodes = 0
    for move ∈ [pls; sls]
        # TODO: undo! moves
        npos = deepcopy(pos)
        nodes += perft(apply!(npos, move, color), opponent(color), depth - 1)
    end

    nodes
end