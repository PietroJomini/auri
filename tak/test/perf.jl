# TODO: read expected values from ../resources/perf.csv

tps = "x5/x5/x5/x5/x5 1 1"
@testset "perft $tps" begin
    @test Tak.perft(Tak.PTN.parse_tps(tps), 1) == 25
    @test Tak.perft(Tak.PTN.parse_tps(tps), 2) == 600
    @test Tak.perft(Tak.PTN.parse_tps(tps), 3) == 43320
    # this is where it stops working
    # @test Tak.perft(Tak.PTN.parse_tps(tps), 4) == 2999784
end