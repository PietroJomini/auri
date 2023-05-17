module PTN

using ..Tak

export square


const COLUMN_NAMES = string.('a':'h')
const ROW_NAMES = string.(1:8)

function square(size::BitboardSize, sq::Square)
    x, y = coordinates(size, sq)
    "$(COLUMN_NAMES[x])$(ROW_NAMES[y])"
end

end