using Base.Iterators, StaticArrays

export Placement, Slide, Move, placements_mask, placements, compute_slides, legal_slides, slides

abstract type Move end

"""
Representation of a placement
"""
struct Placement <: Move
    size::BitboardSize
    at::Square
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)
placements(pos::Position) = map(sq -> Placement(pos.size, sq), placements_mask(pos))

Base.show(io::IO, pl::Placement) = print(io, "Placement($(Tak.PTN.square(pl.size, pl.at)))")

# TODO: pack `height` in a UInt32 representing 8 4-bits stacks
# and precompile all the possibilities
"""
Reprsentation of a slide
"""
struct Slide <: Move
    oigin::Square
    direction::Direction
    length::UInt8
    height::SVector{8,UInt8}
end

function compute_slides(height::Int,  cap::Int, previous::Vector{Int} = Vector{Int}())
    if height == 0 || length(previous) == cap
        return [previous]
    # ensure that the slide will always consume all the stones
    elseif height == 1 || length(previous) == cap - 1
        return [[previous; height]]
    else
        min_drop = min(1, length(previous))
        return [((compute_slides(height - dropped, cap, [previous; dropped]) for dropped in min_drop:height)...)...]
    end
end

function legal_slides(pos::Position, dir::Direction, at::Square)
    height = min(pos.size, pos.heights[at.index])

    cap = 1
    nat = at
    nnat = slide(pos.size, nat, dir)
    while dir ∈ neighbors(pos.size, nat) && nnat ∉ (pos.walls ∪ pos.caps)
        cap += 1
        nat = nnat
        nnat = slide(pos.size, nat, dir)
    end

    slides = []

    # check for flattening possibilities
    if nnat.index ≠ -1 && at ∈ pos.caps && nnat ∈ pos.walls
        slides =  filter(hs -> hs[end] == 1 || length(hs) ≤ cap, compute_slides(Int(height), cap + 1))
    else
        slides =  compute_slides(Int(height), cap)
    end

    # disallow dropping all stones on the origin square
    return filter(s -> length(s) > 1, slides)
end

function slides(pos::Position, at::Square)::Vector{Slide}
    dirs = neighbors(pos.size, at)
    moves = []

    for dir in dirs
        for s in legal_slides(pos, dir, at)
            push!(moves, Slide(at, dir, length(s), SVector{8}([s; (0 for _ in length(s):7)...])))
        end
    end

    moves
end