export Syntax, STD
export square, move

struct Syntax
    row_separator::Char
    square_separator::Char
    empty::Char
    players::Dict{Player,Char}
    pieces::Dict{Piece,Char}
    columns::SVector{8,Char}
    rows::SVector{8,Char}
    directions::Dict{Direction,Char}
    flattens::Char
    tak::String
    tinue::String
    evaluation::SVector{6, String}
end

const STD = Syntax(
    '/',
    ',',
    'x',
    Dict(White => '1', Black => '2'),
    Dict(Capstone => 'C', Wall => 'S'),
    SVector{8}('a':'h'),
    SVector{8}('1':'8'),
    Dict(DIR_N => '+', DIR_S => '-', DIR_W => '<', DIR_E => '>'),
    '*',
    "'",
    "''",
    SVector{6}(["??", "?", "?!", "!?", "!", "!!"])
)

function square(size::BitboardSize, sq::Square, syntax::Syntax=STD)
    r, c = coordinates(size, sq)
    "$(syntax.columns[c])$(syntax.rows[r])"
end

function move(size::BitboardSize, p::Placement, syntax::Syntax=STD)
    k = p.kind != Flatstone ? syntax.pieces[p.kind] : ""
    s = square(size, p.at, syntax)
    "$k$s"
end

function move(size::BitboardSize, slide::Slide, long::Bool=false, syntax::Syntax=STD)
    l = sum(slide.heights)
    o = square(size, slide.origin, syntax)
    d = syntax.directions[slide.direction]
    h = join(Int.(slide.heights[begin:slide.length]))

    !long && slide.length == 1 && (h = "")
    !long && l == 1 && (l = "")
    
    return "$l$o$d$h"
end