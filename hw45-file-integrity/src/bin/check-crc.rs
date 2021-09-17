use hw45::crc16sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let data = std::fs::read(path).unwrap();
    let checksum = crc16sum(&data);
    println!("crc16sum {} {:#06x} {:#018b}", checksum, checksum, checksum);
}
