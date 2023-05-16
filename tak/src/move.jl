export Placement, Slide, Move, placements_mask, placements

abstract type Move end

struct Placement <: Move
    position::Bitboard
end

struct Slide <: Move
    oigin::Bitboard
    direction::UInt8
    length::UInt8
    height::SVector{8,UInt8}
end

placements_mask(position::Position) = ~(position.white ∪ position.black)
placements(position::Position) = map(Placement, placements_mask(position))

Base.show(io::IO, placement::Placement) = print(io, "Placement(0x$(string(placement.position.raw, base=16, pad=16)))")
