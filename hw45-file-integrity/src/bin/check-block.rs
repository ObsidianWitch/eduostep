use hw45::xor8sum;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path_in = args.get(1).expect("missing operand: in_data_path");
    let path_out = args.get(2).expect("missing operand: out_checksums_path");
    let data = std::fs::read(path_in).unwrap();
    let checksums: Vec<u8> = data.chunks(4096).map(|chunk| xor8sum(chunk)).collect();
    std::fs::write(path_out, checksums).unwrap();
}
