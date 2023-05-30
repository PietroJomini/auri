"""
Print the tps of ARGS[1] random game sof size ARGS[2]
as csv in the form of "size, tps"
"""

using Tak, Tak.PTN, Tak.Moves

# TODO add end causes
function random_game(s::Size)
    p = startposition(s)
    for _ ∈ 1:rand(1:50)
        apply!(p, rand(moves(p)))
    end
    p
end

if abspath(PROGRAM_FILE) == @__FILE__
    amount = length(ARGS) ≥ 1 ? parse(Int, ARGS[1]) : 1
    s = Size(length(ARGS) ≥ 2 ? parse(Int, ARGS[2]) : 5)
    foreach(_ -> println("$s, \"$(to_tps(random_game(s)))\""), 1:amount)
end