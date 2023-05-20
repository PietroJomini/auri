export has_route

# TODO: needs a LOT of tests
function has_route(bb::Bitboard)


    # maybe abusing python-like stuff a bit too much =?
    hooks = [(filter(sq -> sq ∈ bb, collect(f(bb.size, 1))), f) for f ∈ (row, column)]

    for (origins, opposite) ∈ hooks
        visited = Tak.empty(bb.size)
        for sq ∈ origins
            if sq ∉ visited
                # check if the road is completed
                opposite(bb.size, sq) == bb.size && return true
                visited = visited ∪ Bitboard(bb.size, sq)

                # propagate neighbors
                nb = map(d -> slide(bb.size, sq, d), neighbors(bb.size, sq))
                nb = filter(n -> n ∈ bb && n ∉ visited, nb)
                append!(origins, nb)
            end
        end
    end

    false
end