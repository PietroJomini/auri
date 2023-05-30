using Tak, Tak.BB

@testset "bitboard/constructors" begin
    @test Bitboard(1).value == 1
    @test Bitboard(BitVector((true, false, true))) == Bitboard(5)
    @test Bitboard(BitVector(fill!(Vector(undef, 64), true))) == Bitboard(typemax(UInt64))
    @test BB.empty().value == 0
    @test universe().value == typemax(UInt64)
    @test universe(BS5).value == 2^25 - 1
end

@testset "bitboard/operations" begin
    # TODO
end

@testset "bitboard/squaresets" begin
    @test square(1) == Bitboard(0b1)
    @test square(5) == Bitboard(0b10000)
    # TODO: row, column
end

@testset "bitboard/square" begin
    @testset let sq = Square(1, 1)
        @test row(sq) == 1
        @test column(sq) == 1
        @testset let nb = neighbors(sq, BS3)
            @test length(nb) == 2
            @test Square(1, 2) ∈ nb
            @test Square(2, 1) ∈ nb
        end
    end
    @testset let sq = Square(2, 3)
        @test row(sq) == 2
        @test column(sq) == 3
        @test ok(sq, BS3)
        @test row(slide(sq, DIR_N)) == 3
        @test column(slide(sq, DIR_N)) == 3
        @test row(slide(sq, DIR_E)) == 2
        @test column(slide(sq, DIR_E)) == 4
        @test row(slide(sq, DIR_S)) == 1
        @test column(slide(sq, DIR_S)) == 3
        @test row(slide(sq, DIR_W)) == 2
        @test column(slide(sq, DIR_W)) == 2
        @testset let nb = neighbors(sq, BS3)
            @test length(nb) == 3
            @test Square(3, 3) ∈ nb
            @test Square(1, 3) ∈ nb
            @test Square(2, 2) ∈ nb
        end
        @testset let nb = neighbors(sq, BS4)
            @test length(nb) == 4
            @test Square(3, 3) ∈ nb
            @test Square(1, 3) ∈ nb
            @test Square(2, 2) ∈ nb
            @test Square(2, 4) ∈ nb
        end
    end
    @testset let sq = Square(8, 8)
        @test row(sq) == 8
        @test column(sq) == 8
        @test ok(sq, BS8)
        @test !ok(sq, BS7)
    end
end