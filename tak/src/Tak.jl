module Tak

include("ruleset.jl")
include("bitboard/mod.jl")
include("position.jl")
include("moves/mod.jl")
include("ptn/mod.jl")
include("routes.jl")

# tests placebo
export hello_tak
hello_tak(size::Size) = size == BS7 ? "spooky blue flame" : "comfy red candle"

end
