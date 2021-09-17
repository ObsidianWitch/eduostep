use hw45::xor8sum;

fn create(in_data: &str, out_checksums: &str) -> std::io::Result<()> {
    let data = std::fs::read(in_data)?;
    let checksums: Vec<u8> = data.chunks(4096).map(xor8sum).collect();
    std::fs::write(out_checksums, checksums)?;
    return Ok(());
}

fn check(in_data: &str, in_checksums: &str) -> std::io::Result<()> {
    let data = std::fs::read(in_data)?;
    let computed_checksums: Vec<u8> = data.chunks(4096).map(xor8sum).collect();
    let saved_checksums = std::fs::read(in_checksums)?;
    assert!(computed_checksums == saved_checksums);
    return Ok(());
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let operation = args.get(1).expect("missing operand: op:create|check");
    let data_path = args.get(2).expect("missing operand: data_path");
    let checksums_path = args.get(3).expect("missing operand: checksums_path");
    match operation.as_str() {
        "create" => create(data_path, checksums_path).unwrap(),
        "check" => check(data_path, checksums_path).unwrap(),
        _ => panic!("incorrect operand: op:create|check"),
    }
}
