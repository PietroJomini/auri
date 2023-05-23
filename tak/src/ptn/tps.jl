export parse_tps, tps

# TODO: technically, this results ina  180° rotated boards. do i care?
function parse_tps(tps::String, syntax::Syntax=STD)
    pieces = split(tps, " ")
    length(pieces) != 3 && throw(ArgumentError("Malformed TPS string"))

    tps, turn, moves = pieces
    move = (parse(Int, moves) - 1) * 2 + parse(Int, turn) - 1

    size = BitboardSize(count(syntax.row_separator, tps) + 1)
    index = 1
    jumping = false
    caps = Tak.empty(size)
    walls = Tak.empty(size)
    stacks = [Tak.empty(BBS_8) for _ in 1:64]
    heights = [UInt8(0) for _ in 1:64]
    ceiling = [-1 for _ in 1:64]

    for c ∈ tps
        if c == syntax.empty
            jumping = true
        elseif c ∈ (syntax.row_separator, syntax.square_separator)
            index += 1
            jumping = false
        elseif c == syntax.pieces[Capstone]
            caps = caps ∪ Tak.square(size, index)
        elseif c == syntax.pieces[Wall]
            walls = walls ∪ Tak.square(size, index)
        elseif jumping && isdigit(c)
            index += parse(Int, c) - 1
        elseif c ∈ (syntax.players[White], syntax.players[Black])
            heights[index] += 1
            value = (c == syntax.players[White]) ? 0 : 1
            stacks[index] = (stacks[index] << 1) + value
            ceiling[index] = value
        end
    end

    white = from_bit_vector(size, ceiling .== 0)
    black = from_bit_vector(size, ceiling .== 1)
    stacks = MVector{64}(stacks)
    heights = MVector{64}(heights)

    Position(size, white, black, caps, walls, heights, stacks, move)
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

                s = Tak.square(position.size, index)
                stack = bitstring(position.stacks[index].raw)[end-height+1:end]

                push!(cell, replace(stack, "0" => syntax.players[White], "1" => syntax.players[Black]))
                push!(cell, s ⊆ position.caps ? syntax.pieces[Capstone] : s ⊆ position.walls ? syntax.pieces[Wall] : "")
                push!(rows[row_index], join(cell))
            end
        end

        if jumps > 0
            push!(rows[row_index], syntax.empty * (jumps == 1 ? "" : string(jumps)))
            jumps = 0
        end
    end

    move = position.move ÷ 2 + 1
    turn = rem(position.move, 2) + 1
    tps = join(join.(rows, syntax.square_separator), syntax.row_separator)

    "$tps $turn $move"
end