using StaticArrays

export Slide, compute_slides, walk, legal_slides, slides, head, apply!

struct Slide
    origin::Square
    direction::Direction
    length::UInt8
    heights::SVector{8,UInt8}
end

fill_sa(s::Vector) = SVector{8, UInt8}([s; (0 for _ in length(s):7)...])
Slide(origin::Square, dir::Direction, heights::Vector) = Slide(origin, dir, length(heights), fill_sa(heights))

# TODO: something wrong, eg for (4, 3) doesn't generate [1, 2], [2, 1]
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

slides(pos::Position) = vcat([slides(pos, sq) for sq ∈ getfield(pos, symbol(turn(pos)))]...)
head(size::BitboardSize, sl::Slide) = reduce((sq, _) -> slide(size, sq, sl.direction), 1:(sl.length); init=sl.origin)

function apply!(pos::Position, sl::Slide)
    at = head(pos.size, sl)

    # if needed, update caps and walls
    sl.origin ∈ pos.caps && (pos.caps = (pos.caps - square(pos.size, sl.origin.index)) ∪ square(pos.size, at.index))
    sl.origin ∈ pos.walls && (pos.walls = (pos.walls - square(pos.size, sl.origin.index)) ∪ square(pos.size, at.index))

    # update the stacks
    for i ∈ 1:sl.length
        h = sl.heights[end-i-(7 - sl.length)]

        # add the pieces to the pos stack
        pos.heights[at.index] += h
        pos.stacks[at.index] = (pos.stacks[at.index] << h) ∪ (pos.stacks[sl.origin.index] ∩ Bitboard(pos.size, 2^h-1))

        # remove pieces from the origin
        pos.heights[sl.origin.index] -= h
        pos.stacks[sl.origin.index] >> h

        # slide the pointer
        at = slide(pos.size, at, inverse(sl.direction))
    end

    # update the move count
    pos.move += 1

    pos
end