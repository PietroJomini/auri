export Piece, Capstone, Flatstone, Wall

abstract type Piece end

struct Capstone <: Piece end
struct Flatstone <: Piece end
struct Wall <: Piece end

P_C = Capstone()
P_F = Flatstone()
P_W = Wall()
