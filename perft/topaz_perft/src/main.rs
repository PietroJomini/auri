use std::env;
use std::io::{self, BufRead};
use topaz_tak::{perft, TakGame};

fn main() {
    let perft_upto = match env::var("PERFT_UPTO") {
        Ok(v) => v.parse::<u16>().unwrap_or(1),
        Err(_) => 1,
    };

    for line in io::stdin().lock().lines() {
        handle_line(line.unwrap(), perft_upto)
    }
}

fn handle_line(tps: String, perft_upto: u16) {
    print!("\"{}\"", tps);
    let mut position = TakGame::try_from_tps(&tps).unwrap();
    for i in 1..(perft_upto + 1) { print!(", {}", perft(&mut position, i)) }
    println!("");
}
