export perft1

struct PerftResult
    total::Int
    nplacements::Int
    nslides::Int
    placements::Vector{Placement}
    slides::Vector{Slide}
end

Base.show(io::IO, pr::PerftResult) = print(io, "PerfResult[$(pr.total); placements=$(pr.nplacements); slides=$(pr.nslides)]")

# TODO: first placement forced to flatstone
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