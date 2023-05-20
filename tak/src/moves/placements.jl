export Placement, placements_mask, placements

struct Placement
    at::Square
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)
placements(pos::Position) = map(sq -> Placement(sq), placements_mask(pos))