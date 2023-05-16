module TPS

using StaticArrays, ..Tak

export parse_tps, tps, Syntax

struct Syntax
    white::Char
    black::Char
    wall::Char
    capstone::Char
    row_separator::Char
    square_separator::Char
    empty::Char
end

const STD = Syntax('1', '2', 'S',  'C', '/', ',', 'x')

function parse_tps(tps::String, syntax::Syntax = STD)
    size = count(syntax.row_separator, tps) + 1
    index = 1
    jumping = false
    caps = Tak.empty(size)
    walls = Tak.empty(size)
    stacks = [Tak.empty() for _ in 1:64]
    heights = [UInt8(0) for _ in 1:64]
    ceiling = [-1 for _ in 1:64]

    for c ∈ tps
        if c == syntax.empty
            jumping = true
        elseif c ∈ (syntax.row_separator, syntax.square_separator)
            index += 1
            jumping = false
        elseif c == syntax.capstone
            caps = caps ∪ square(index, size)
        elseif c == syntax.wall
            walls = walls ∪ square(index, size)
        elseif jumping && isdigit(c)
            index += parse(Int, c) - 1
        elseif c ∈ (syntax.white, syntax.black)
            heights[index] += 1
            value = (c == syntax.white) ? 0 : 1
            stacks[index] = (stacks[index] << 1) + value
            ceiling[index] = value
        end
    end

    white = fromBitVector(ceiling .== 0, size)
    black = fromBitVector(ceiling .== 1, size)
    stacks = SVector{64}(stacks)
    heights = SVector{64}(heights)

    Position(size, white, black, caps, walls, heights, stacks)
end

function tps(position::Position, syntax::Syntax=STD)
    rows = [[] for _ in 1:position.size]
    jumps = 0

    for row_index ∈ 1:position.size
        for column_index ∈ 1:position.size
            cell = []
            index = (row_index - 1) * position.size + column_index
            height = position.heights[index]
            if height == 0
                jumps += 1
            else
                if jumps > 0
                    push!(rows[row_index], syntax.empty * (jumps == 1 ? "" : string(jumps)))
                    jumps = 0
                end

                s = square(index, position.size)
                stack = bitstring(position.stacks[index].raw)[end-height+1:end]

                push!(cell, replace(stack, "0" => syntax.white, "1" => syntax.black))
                push!(cell, s ∈ position.caps ? syntax.capstone : s ∈ position.walls ? syntax.wall : "")
                push!(rows[row_index], join(cell))
            end
        end

        if jumps > 0
            push!(rows[row_index], syntax.empty * (jumps == 1 ? "" : string(jumps)))
            jumps = 0
        end
    end

    join(join.(rows, syntax.square_separator), syntax.row_separator)
end

end