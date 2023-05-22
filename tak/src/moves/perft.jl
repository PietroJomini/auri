export perf1

struct PerfResult
    total::Int
    nplacements::Int
    nslides::Int
    placements::Vector{Placement}
    slides::Vector{Slide}
end

Base.show(io::IO, pr::PerfResult) = print(io, "PerfResult[$(pr.total); placements=$(pr.nplacements); slides=$(pr.nslides)]")

# TODO: i shouldn't have to add remaining, bu i should get them from the position
# eg first placement forced to a flatstone
function perf1(pos::Position, remaining::Vector{Piece}=[Wall, Capstone, Flatstone])
    pls = placements(pos, remaining)
    sls = []

    # should i differ between perf-ing for white and black?
    for p ∈ (pos.white ∪ pos.black)
        append!(sls, slides(pos, p))
    end

    npls = length(pls)
    nsls = length(sls)
    PerfResult(npls + nsls, npls, nsls, pls, sls)
end