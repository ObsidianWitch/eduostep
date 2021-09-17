use hw45::xor8sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let data = std::fs::read(path).unwrap(); // no buffering for simplicity
    let checksum = xor8sum(&data);
    println!("xor8sum {} {:#04x} {:#010b}", checksum, checksum, checksum);
}
