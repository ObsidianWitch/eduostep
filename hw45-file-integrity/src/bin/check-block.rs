use hw45::xor8sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path_in = args.get(1).expect("missing operand: path_in");
    let data = std::fs::read(path_in).unwrap();
    for chunk in data.chunks(4096) {
        let checksum = xor8sum(chunk);
        println!("xor8sum {} {:#04x} {:#010b}", checksum, checksum, checksum);
    }
}
