using StaticArrays

export Slide, compute_slides, walk, legal_slides, slides

struct Slide
    origin::Square
    direction::Direction
    length::UInt8
    height::SVector{8,UInt8}
end

fill_sa(s::Vector) = SVector{8, UInt8}([s; (0 for _ in length(s):7)...])
Slide(origin::Square, dir::Direction, height::Vector) = Slide(origin, dir, length(height), fill_sa(height))

function compute_slides(height::Int, cap::Int, previous::Vector{Int} = Vector{Int}())::Vector{Vector{Int}}
    cap == 0 && return []
    (height == 0 || length(previous) == cap) && return [previous]
    leafs = map(i -> compute_slides(height - i, cap, [previous; i]), 1:height)
    return unique([[height], (leafs...)...])
end

function walk(pos::Position, dir::Direction, at::Square, trace::Int = 0)
    dir ∉ neighbors(pos.size, at) && return trace, false
    next = slide(pos.size, at, dir)
    next ∈ pos.caps && return trace, false
    next ∈ pos.walls && return trace, true
    return walk(pos, dir, next, trace + 1)
end

function legal_slides(pos::Position, dir::Direction, at::Square)
    height = min(pos.size, pos.heights[at.index])
    cap, niw = walk(pos, dir, at)
    slides = compute_slides(Int(height), cap + (niw ? 1 : 0))
    niw ? filter(hs -> hs[end] == 1 || length(hs) ≤ cap, slides) : slides
end

function slides(pos::Position, at::Square)
    dirs = neighbors(pos.size, at)
    sls = [[(dir, sl) for sl ∈ legal_slides(pos, dir, at)] for dir ∈ dirs]
    return [Slide(at, dir, sl) for (dir, sl) ∈ vcat(sls...)]
end