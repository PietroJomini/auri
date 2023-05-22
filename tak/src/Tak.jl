module Tak

include("ruleset.jl")
include("bitboard.jl")
include("position.jl")
include("moves/placements.jl")
include("moves/slides.jl")
include("moves/perft.jl")
include("routes.jl")
include("ptn/Ptn.jl")

# tests placebo
export hello_tak
hello_tak(size::BitboardSize) = size == BBS_7 ? "spooky blue flame" : "comfy red candle"

end
