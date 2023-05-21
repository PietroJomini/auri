use std::io::{ self, BufRead };
use tiltak::position::Position;
use pgn_traits::PgnPosition;

/*
TODOS:
- actually understand what tiltak's perft does
    - why does the move number changes the result?
x fetch tpss from file -> cargo run < positions.tps
x write perf results to csv
*/

fn main() {
    for line in io::stdin().lock().lines() {
        perft(line.unwrap())
    }
}

fn perft(tps:String) {
    // TODO: boards with different sizes
    let mut position = <Position<5>>::from_fen(&tps).unwrap();
    print!("{}", tps);
    for i in 1..5 { print!(";{}", position.bulk_perft(i)) }
    println!();
}