using .BB

export has_route

function has_route(pos::Position, color::Player)
    bb = getfield(pos, symbol(color)) - pos.walls
    println(IOContext(stdout, :size=>5), bb)
    @show color

    # maybe abusing python-like stuff a bit too much =?
    hooks = map(f -> (filter(i -> square(i) ⊆ bb, collect(f(pos.size, 1))), f), (row, column))

    for (origins, opposite) ∈ hooks
        opposite = opposite(pos.size, pos.size)
        visited = BB.empty()
        for i ∈ origins
            if (sbb = square(i)) ⊈ visited
                # check if the road is completed
                sbb ⊆ opposite && return true

                # update visited map
                visited = visited ∪ sbb

                # propagate neighbors
                nb = map(n -> index(n[2], pos.size), neighbors(Square(i, pos.size), pos.size))
                nb = filter(n -> (sq = square(n)) ⊆ bb, nb)
                @show nb
                append!(origins, nb)
            end
        end
    end

    false
end