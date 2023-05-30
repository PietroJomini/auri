using Tak
using Test

@testset "assert environment" begin
    @test true
    @test 1 + 1 == 2
    @test Tak.hello_tak(BS5) == "comfy red candle"
    @test Tak.hello_tak(BS7) == "spooky blue flame"
end

include("bitboard.jl")

# TODO: moves, ptn, position, ruleset, routes