extern crate tiltak;

use tiltak::position::Position;
use pgn_traits::PgnPosition;

/*
TODOS:
- actually understand what tiltak's perft does
    - why does the move number changes the result?
- fetch tpss from file
- write perf results to csv
*/

fn main() {
    // let tps = String::from("x5/x5/x5/x5/x5 1 1");
    let tps = String::from("x3,12,2S/x,22S,22C,11,21/121,212,12,1121C,1212S/21S,1,21,211S,12S/x,21S,2,x2 1 1");
    // let mut position: Position<5> = Position::default();

    let mut position = <Position<5>>::from_fen(&tps).unwrap();

    println!("{}", position.bulk_perft(1));
}
