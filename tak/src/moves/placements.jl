export Placement, placements_mask, placements, apply!

struct Placement
    at::Square
    kind::Piece
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)

placements(pos::Position, kinds::Vector{Piece}) = placements(pos, flats=Flatstone ∈ kinds, walls=Wall ∈ kinds, caps=Capstone ∈ kinds)
function placements(pos::Position; flats::Bool = true, walls::Bool = true, caps::Bool = true)
    mask = collect(placements_mask(pos))
    # for the first move we can only play a flatstone
    kinds = (pos.white ∪ pos.white).raw > 0 ? [(Flatstone, flats), (Wall, walls), (Capstone, caps)] : [(Flatstone, flats)]
    vcat([map(sq -> Placement(sq, kind), mask) for (kind, allowed) in kinds if allowed]...)
end

function apply!(pos::Position, pl::Placement, color::Player)
    at = Bitboard(pos.size, pl.at)
    setfield!(pos, symbol(color), getfield(pos, symbol(color)) ∪ at)
    pos.heights[pl.at.index] = 1
    pos.stacks[pl.at.index] += Int(color)
    pos
end