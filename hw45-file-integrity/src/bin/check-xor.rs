use std::fs::File;
use std::io::BufReader;
use hw45::xor8sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let file = File::open(path).unwrap();
    let mut reader = BufReader::new(file);
    let checksum = xor8sum(&mut reader);
    println!("xor8sum {} {:#04x} {:#010b}", checksum, checksum, checksum);
}
