export Syntax, STD

struct Syntax
    row_separator::Char
    square_separator::Char
    empty_cell::Char
    players::Dict{Player,Char}
    pieces::Dict{Piece,Char}
    columns::SVector{8,Char}
    rows::SVector{8,Char}
    directions::Dict{Direction,Char}
    flattens::String
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
    "*",
    "'",
    "''",
    SVector{6}(["??", "?", "?!", "!?", "!", "!!"])
)