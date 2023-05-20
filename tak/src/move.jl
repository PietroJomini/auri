using Base.Iterators, StaticArrays

export Placement, Slide, Move, placements_mask, placements, compute_slides, legal_slides, slides

abstract type Move end

"""
Representation of a placement
"""
struct Placement <: Move
    at::Square
end

placements_mask(pos::Position) = ~(pos.white ∪ pos.black)
placements(pos::Position) = map(sq -> Placement(sq), placements_mask(pos))

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

function compute_slides(height::Int, cap::Int, previous::Vector{Int} = Vector{Int}())
    (height == 0 || length(previous) == cap) && return [previous]

    leafs = map(i -> compute_slides(height - i, cap, [previous; i]), 1:height)
    return unique([[height], (leafs...)...])
end

function legal_slides(pos::Position, dir::Direction, at::Square)
    height = min(pos.size, pos.heights[at.index])

    cap = 0
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

    return slides
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