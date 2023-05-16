export tps2position, position2tps

function tps2position(tps::String)
    size = count('/', tps) + 1
    index = 1
    jumping = false
    caps = empty(size)
    walls = empty(size)
    stacks = [empty() for _ in 1:64]
    heights = [UInt8(0) for _ in 1:64]
    ceiling = [0 for _ in 1:64]

    for c ∈ tps
        if c == 'x'
            jumping = true
        elseif c ∈ (',', '/')
            index += 1
            jumping = false
        elseif c == 'C'
            caps = caps ∪ square(index, size)
        elseif c == 'S'
            walls = walls ∪ square(index, size)
        elseif jumping && isdigit(c)
            index += parse(Int, c) - 1
        elseif isdigit(c)
            value = parse(Int, c)
            heights[index] += 1
            stacks[index] = (stacks[index] << 1) + (value - 1)
            ceiling[index] = value
        end
    end

    white = fromBitVector(ceiling .== 1, size)
    black = fromBitVector(ceiling .!= 2, size)
    stacks = SVector{64}(stacks)
    heights = SVector{64}(heights)

    Position(size, white, black, caps, walls, heights, stacks)
end

function position2tps(position::Position)
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
                    push!(rows[row_index], "x" * (jumps == 1 ? "" : string(jumps)))
                    jumps = 0
                end

                s = square(index, position.size)
                stack = bitstring(position.stacks[index].raw)[end-height+1:end]

                push!(cell, replace(stack, "0" => "1", "1" => "2"))
                push!(cell, s ∈ position.caps ? "C" : s ∈ position.walls ? "S" : "")
                push!(rows[row_index], join(cell))
            end
        end

        if jumps > 0
            push!(rows[row_index], "x" * (jumps == 1 ? "" : string(jumps)))
            jumps = 0
        end
    end

    join(join.(rows, ","), "/")
end
