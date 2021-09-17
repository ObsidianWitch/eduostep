use hw45::xor8sum;

fn create(in_data: &str, out_checksums: &str) -> std::io::Result<()> {
    let data = std::fs::read(in_data)?;
    let checksums: Vec<u8> = data.chunks(4096).map(xor8sum).collect();
    std::fs::write(out_checksums, checksums)?;
    return Ok(());
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path_in = args.get(1).expect("missing operand: in_data_path");
    let path_out = args.get(2).expect("missing operand: out_checksums_path");
    create(path_in, path_out).unwrap();
}
