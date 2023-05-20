export Placement, placements_mask, placements

struct Placement
    at::Square
    kind::Piece
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)

placements(pos::Position, kinds::Vector{Piece}) = placements(pos, flats=Flatstone ∈ kinds, walls=Wall ∈ kinds, caps=Capstone ∈ kinds)
function placements(pos::Position; flats::Bool = true, walls::Bool = true, caps::Bool = true)
    mask = collect(placements_mask(pos))
    kinds = [(Flatstone, flats), (Wall, walls), (Capstone, caps)]
    vcat([map(sq -> Placement(sq, kind), mask) for (kind, allowed) in kinds if allowed]...)
end
