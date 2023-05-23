export Placement, placements_mask, placements, apply!

struct Placement
    at::Square
    kind::Piece
    color::Player
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)
placement_color(pos::Position) = pos.move ≤ 1 ? opponent(turn(pos)) : turn(pos)

function placements(pos::Position)
    color = placement_color(pos)
    mask = collect(placements_mask(pos))
    remp = getfield(stats(pos), symbol(color)) .< SETUP[pos.size]
    kinds = [(Flatstone, remp[1]), (Wall, remp[1]), (Capstone, remp[1])]
    kinds = pos.move > 1 ? kinds : [(Flatstone, true)]
    vcat([map(sq -> Placement(sq, kind, color), mask) for (kind, allowed) ∈ kinds if allowed]...)
end

function apply!(pos::Position, pl::Placement)
    at = Bitboard(pos.size, pl.at)
    setfield!(pos, symbol(pl.color), getfield(pos, symbol(pl.color)) ∪ at)
    pos.heights[pl.at.index] = 1
    pos.stacks[pl.at.index] += Int(pl.color)
    pos.move += 1
    pos
end