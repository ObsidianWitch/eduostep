use std::io::{prelude::*, BufReader};
use std::fs::File;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let file = File::open(path).unwrap();
    let file = BufReader::new(file);
    let checksum = file.bytes().fold((0u16, 0u16), |acc, b| {
        let s1 = (acc.0 + b.unwrap() as u16) % 255;
        let s2 = (acc.1 + s1) % 255;
        return (s1, s2);
    });
    println!("fletcher8sum {},{} {:#04x},{:#04x} {:#010b},{:#010b}",
        checksum.0, checksum.1, checksum.0, checksum.1, checksum.0, checksum.1);
}
