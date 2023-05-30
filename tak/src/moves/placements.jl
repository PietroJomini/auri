export Placement, placements, apply!

struct Placement <: Move
    at::Square
    kind::Piece
    color::Player
end

placements_mask(p::Position) = ~(p.white ∪ p.black) ∩ universe(p.size)
placement_color(p::Position) = p.move ≤ 1 ? opponent(turn(p)) : turn(p)

function placements(p::Position)
    color = placement_color(p)
    mask = Square.(collect(placements_mask(p)), p.size)
    kinds = [Flatstone]
    if p.move > 1
        remp = getfield(stats(p), symbol(color)) .< SETUP[p.size]
        kinds = filter(p -> remp[Int(shape(p))], instances(Piece))
    end
    vcat(map(k -> Placement.(mask, k, color), kinds)...)
end

function apply!(pos::Position, pl::Placement)
    atindex = index(pl.at, pos.size)
    at = square(atindex)
    color = symbol(pl.color)
    setfield!(pos, color, getfield(pos, color) ∪ at)
    pos.heights[atindex] = 1
    pos.stacks[atindex] += Int(pl.color)
    pos.move += 1
    pos
end