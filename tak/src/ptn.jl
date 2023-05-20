module PTN

using ..Tak

export square, move


const COLUMN_NAMES = string.('a':'h')
const ROW_NAMES = string.(1:8)
const DIRECTIONS = Dict(DIR_N => "+", DIR_S => "-", DIR_W => "<", DIR_E => ">")

function square(size::BitboardSize, sq::Square)
    x, y = coordinates(size, sq)
    "$(COLUMN_NAMES[x])$(ROW_NAMES[y])"
end

move(size::BitboardSize, placement::Placement) = square(size, placement.at)
function move(size::BitboardSize, slide::Slide, long::Bool = false)
    l = sum(slide.height)
    o = square(size, slide.origin)
    d = DIRECTIONS[slide.direction]
    h = join(Int.(slide.height[begin:slide.length]))

    if long && l == 1
        h = ""
        l = ""
    end

    return "$l$o$d$h"
end

end