export from_tps, to_tps

# TODO: somehow check if the tps is correct, other than the malformed 3-chunks check
function from_tps(tps::String, syntax::Syntax=STD)
    pieces = split(tps, " ")

    # TODO: better and generalized error handling system?
    length(pieces) != 3 && throw(ArgumentError("Malformed TPS string"))

    tps, turn, moves = pieces

    p = startposition(Tak.Size(count(syntax.row_separator, tps) + 1))
    p.move = (parse(Int, moves) - 1) * 2 + parse(Int, turn) - 1
    
    index = 1
    jumping = false
    ceiling = [-1 for _ in 1:64]

    for c ∈ tps
        if c == syntax.empty_cell
            jumping = true
        elseif c ∈ (syntax.row_separator, syntax.square_separator)
            index += 1
            jumping = false
        elseif c == syntax.pieces[Capstone]
            p.caps = p.caps ∪ BB.square(index)
        elseif c == syntax.pieces[Wall]
            p.walls = p.walls ∪ BB.square(index)
        elseif jumping && isdigit(c)
            index += parse(Int, c) - 1
        elseif c ∈ (syntax.players[White], syntax.players[Black])
            p.heights[index] += 1
            value = (c == syntax.players[White]) ? 0 : 1
            p.stacks[index] = (p.stacks[index] << 1) + value
            ceiling[index] = value
        end
    end

    p.white = Bitboard(ceiling .== 0)
    p.black = Bitboard(ceiling .== 1)
    p
end

function to_tps(position::Position, syntax::Syntax=STD)
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
                    push!(rows[row_index], syntax.empty_cell * (jumps == 1 ? "" : string(jumps)))
                    jumps = 0
                end

                s = Tak.square(index)
                stack = bitstring(position.stacks[index].value)[end-height+1:end]

                push!(cell, replace(stack, "0" => syntax.players[White], "1" => syntax.players[Black]))
                push!(cell, s ⊆ position.caps ? syntax.pieces[Capstone] : s ⊆ position.walls ? syntax.pieces[Wall] : "")
                push!(rows[row_index], join(cell))
            end
        end

        if jumps > 0
            push!(rows[row_index], syntax.empty_cell * (jumps == 1 ? "" : string(jumps)))
            jumps = 0
        end
    end

    move = position.move ÷ 2 + 1
    turn = rem(position.move, 2) + 1
    tps = join(join.(rows, syntax.square_separator), syntax.row_separator)

    "$tps $turn $move"
end