use pgn_traits::PgnPosition;
use std::env;
use std::io::{self, BufRead};
use std::time::Instant;
use tiltak::position::Position;

fn main() {
    let perft_upto = match env::var("PERFT_UPTO") {
        Ok(v) => v.parse::<u16>().unwrap_or(1),
        Err(_) => 1,
    };

    for line in io::stdin().lock().lines() {
        handle_line(line.unwrap(), perft_upto);
    }
}

fn handle_line(tps: String, perft_upto: u16) {
    print!("\"{}\"", tps);
    let size: usize = tps.matches("/").count() + 1;

    // this is ugly, can i do better?
    // i hate rust types :(
    if size == 3 {
        let mut position = <Position<3>>::from_fen(&tps).unwrap();
        for i in 1..(perft_upto + 1) {
            let before = Instant::now();
            print!(", {}, {}", position.bulk_perft(i), before.elapsed().as_micros())
        }
    } else if size == 4 {
        let mut position = <Position<4>>::from_fen(&tps).unwrap();
        for i in 1..(perft_upto + 1) {
            let before = Instant::now();
            print!(", {}, {}", position.bulk_perft(i), before.elapsed().as_micros())
        }
    } else if size == 5 {
        let mut position = <Position<5>>::from_fen(&tps).unwrap();
        for i in 1..(perft_upto + 1) {
            let before = Instant::now();
            print!(", {}, {}", position.bulk_perft(i), before.elapsed().as_micros())
        }
    } else if size == 6 {
        let mut position = <Position<6>>::from_fen(&tps).unwrap();
        for i in 1..(perft_upto + 1) {
            let before = Instant::now();
            print!(", {}, {}", position.bulk_perft(i), before.elapsed().as_micros())
        }
    } else if size == 7 {
        let mut position = <Position<7>>::from_fen(&tps).unwrap();
        for i in 1..(perft_upto + 1) {
            let before = Instant::now();
            print!(", {}, {}", position.bulk_perft(i), before.elapsed().as_micros())
        }
    } else if size == 8 {
        let mut position = <Position<8>>::from_fen(&tps).unwrap();
        for i in 1..(perft_upto + 1) {
            let before = Instant::now();
            print!(", {}, {}", position.bulk_perft(i), before.elapsed().as_micros())
        }
    }

    println!();
}
