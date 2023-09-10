using StaticArrays

export Slide, slides, apply!

struct Slide <: Move
    origin::Square
    direction::Direction
    length::UInt8
    heights::SVector{8,UInt8}
    flattens::Bool
end

fill_sa(s::Vector) = SVector{8, UInt8}([s; (0 for _ in length(s):7)...])
Slide(origin::Square, dir::Direction, heights::Vector, flattens::Bool) = Slide(origin, dir, length(heights), fill_sa(heights), flattens)

# TODO: should i precompile this?
# max height = 50
# max cap = 8
function compute_slides(height::Int, cap::Int, previous::Vector{Int} = Vector{Int}())::Vector{Vector{Int}}
    cap == 0 && return []
    (height == 0 || length(previous) == cap) && return [previous]
    leafs = map(i -> compute_slides(height - i, cap, [previous; i]), 1:height)
    return unique!(vcat([[height], (leafs...)...], length(previous) == 0 ? [] : [previous]))
end

function walk(pos::Position, dir::Direction, at::Square, trace::Int = 0)
    next = slide(at, dir)
    !ok(next, pos.size) && return trace, false
    BB.in(next, pos.caps, pos.size) && return trace, false
    BB.in(next, pos.walls, pos.size) && return trace, true
    return walk(pos, dir, next, trace + 1)
end

function slides(pos::Position, at::Square, dir::Direction)
    height = min(pos.size, pos.heights[index(at, pos.size)])
    cap, niw = walk(pos, dir, at)

    # allow flattening only if the ceiling is a capstone
    flattens = niw && BB.in(at, pos.caps, pos.size)

    slides = compute_slides(Int(height), cap + niw)
    (flattens ? filter(hs -> hs[end] == 1 || length(hs) ≤ cap, slides) : slides, flattens)
end

function slides(pos::Position, at::Square)::Vector{Slide}
    asls = []
    for dir ∈ getindex.(neighbors(at, pos.size), 1)
        sls, flattens = slides(pos, at, dir)
        append!(asls, map(sl -> Slide(at, dir, sl, flattens), sls))
    end
    asls
end

function slides(pos::Position)::Vector{Slide}
    mask = getfield(pos, symbol(turn(pos)))
    vcat(map(sq -> slides(pos, Square(sq, pos.size)), mask)...)
end

# TODO: update color bitboards
function apply!(pos::Position, sl::Slide)
    at = slide(sl.origin, sl.direction, sl.length)
    atindex = index(at, pos.size)
    orindex = index(sl.origin, pos.size)
    orb = square(orindex)
    atb = square(atindex)
    idir = -sl.direction

    # if needed, update caps and walls
    # i could use in(...), but it would compute twice index(...), making it slower
    orb ⊆ pos.caps && (pos.caps = (pos.caps - orb) ∪ atb)
    orb ⊆ pos.walls && (pos.walls = (pos.walls - orb) ∪ atb)
    atb ⊆ pos.walls && (pos.walls = pos.walls - atb)

    # update the stacks
    for i ∈ 1:sl.length
        h = sl.heights[end-i-(7 - sl.length)]

        # add the pieces to the pos stack
        pos.heights[atindex] += h
        pos.stacks[atindex] = (pos.stacks[atindex] << h) ∪ (pos.stacks[orindex] ∩ universe(Int(h)))

        # remove pieces from the origin
        pos.heights[orindex] -= h
        pos.stacks[orindex] = pos.stacks[orindex] >> h

        # slide the pointer
        at = slide(at, idir)
        atindex = index(at, pos.size)
    end

    # update the move count
    pos.move += 1

    pos
end

function undo!(pos::Position, sl::Slide)
    destination = slide(sl.origin, sl.direction, sl.length)
    dindex = index(destination, pos.size)
    oindex = index(sl.origin, pos.size)
    dbb = square(dindex)
    obb = square(oindex)

    # if needed, update caps and walls
    dbb ⊆ pos.caps && (pos.caps = (pos.caps - dbb) ∪ obb)
    dbb ⊆ pos.walls && (pos.walls = (pos.walls - dbb) ∪ obb)
    sl.flattens && (pos.walls = (pos.walls ∪ dbb))

    at = slide(sl.origin, sl.direction)
    atindex = index(at, pos.size)

    #update the stacks
    for i ∈ 1:sl.length
        h = sl.heights[i]

        # add the pieces to the origin
        pos.heights[oindex] += h
        pos.stacks[oindex] = (pos.stacks[oindex] << h) ∪ (pos.stacks[atindex] ∩ universe(Int(h)))

        # remove the pieces from the pointer
        pos.heights[atindex] -= h
        pos.stacks[atindex] = pos.stacks[atindex] >> h
        
        # slide the pointer
        at = slide(at, sl.direction)
        atindex = index(at, pos.size)
    end

    #update the move count
    pos.move -= 1

    pos
end