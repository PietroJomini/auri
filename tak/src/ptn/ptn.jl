export square, move

square(sq::Square, syntax::Syntax=STD) = "$(syntax.columns[column(sq)])$(syntax.rows[row(sq)])"

function move(p::Placement, long::Bool=false, syntax::Syntax=STD)
    k = p.kind != Flatstone ? syntax.pieces[p.kind] : ""
    s = square(p.at, syntax)
    c = syntax.players[p.color]

    !long && (c = "")

    "$c$k$s"
end

function move(slide::Slide, long::Bool=false, syntax::Syntax=STD)
    l = sum(slide.heights)
    o = square(slide.origin, syntax)
    d = syntax.directions[slide.direction]
    h = join(Int.(slide.heights[begin:slide.length]))
    f = slide.flattens ? syntax.flattens : ""

    !long && slide.length == 1 && (h = "")
    !long && l == 1 && (l = "")
    
    return "$l$o$d$h$f"
end