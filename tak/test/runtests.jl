using Tak
using Test

@testset "assert environment" begin
    @test true
    @test 1 + 1 == 2
    @test Tak.hello_tak(BBS_5) == "comfy red candle"
    @test Tak.hello_tak(BBS_7) == "spooky blue flame"
end

include("perf.jl")