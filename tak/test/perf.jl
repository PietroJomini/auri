# TODO: read expected values from ../resources/perf.csv

ptn = "x5/x5/x5/x5/x5"
@testset "$ptn" begin
    @test Tak.perf1(from_ptn(ptn)) == 25
end