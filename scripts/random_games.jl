"""
Print the tps of ARGS[1] random game sof size ARGS[2]
as csv in the form of "size, tps"
"""

using Tak, Tak.PTN

random_games(amount::Int, size::BitboardSize) = map(_ -> random(size), 1:amount)
print_games(games::Vector{Position}) = foreach(p -> println("$bbs, \"$(tps(random(bbs))) 1 1\""), games)

if abspath(PROGRAM_FILE) == @__FILE__
    amount = length(ARGS) ≥ 1 ? parse(Int, ARGS[1]) : 1
    bbs = BitboardSize(length(ARGS) ≥ 2 ? parse(Int, ARGS[2]) : 5)
    print_games(random_games(amount, bbs))
end