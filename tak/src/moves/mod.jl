module Moves

using ..Tak, ..Tak.BB

export Move, moves
abstract type Move end

Base.show(io::IO, sl::Move) = print(io, Tak.PTN.move(sl, get(io, :long, false)))

include("placements.jl")
include("slides.jl")

"""
Generate all possible moves in a given position
"""
moves(pos::Position)::Vector{Move} = pos.move ≥ 2 ? [placements(pos); slides(pos);] : placements(pos)

include("perft.jl")

end