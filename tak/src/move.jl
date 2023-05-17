export Placement, Slide, Move, placements_mask, placements

abstract type Move end

struct Placement <: Move
    size::BitboardSize
    at::Square
end

struct Slide <: Move
    oigin::Bitboard
    direction::UInt8
    length::UInt8
    height::SVector{8,UInt8}
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)
placements(pos::Position) = map(sq -> Placement(pos.size, sq), placements_mask(pos))

Base.show(io::IO, pl::Placement) = print(io, "Placement($(Tak.PTN.square(pl.size, pl.at)))")