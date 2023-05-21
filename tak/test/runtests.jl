using Tak
using Test

@testset "assert environment" begin
    @test true
    @test 1 + 1 == 2
end

include("perf.jl")