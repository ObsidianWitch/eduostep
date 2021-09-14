use std::fs::File;
use std::io::BufReader;
use hw45::fletcher8sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let file = File::open(path).unwrap();
    let mut reader = BufReader::new(file);
    let checksum = fletcher8sum(&mut reader);
    println!("fletcher8sum {},{} {:#04x},{:#04x} {:#010b},{:#010b}",
        checksum.0, checksum.1, checksum.0, checksum.1, checksum.0, checksum.1);
}
