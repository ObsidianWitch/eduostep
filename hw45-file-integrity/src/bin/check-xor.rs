use hw45::xor8sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let checksum = xor8sum(path);
    println!("xor8sum {} {:#04x} {:#010b}", checksum, checksum, checksum);
}
