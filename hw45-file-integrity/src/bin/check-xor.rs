use std::io::{prelude::*, BufReader};
use std::fs::File;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let file = File::open(path).unwrap();
    let file = BufReader::new(file);
    let checksum = file.bytes().fold(0u8, |acc, b| acc ^ b.unwrap());
    println!("xor8sum {} {:#04x} {:#010b}", checksum, checksum, checksum);
}
