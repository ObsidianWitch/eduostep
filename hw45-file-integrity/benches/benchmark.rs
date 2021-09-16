use std::path::Path;
use std::process::Command;
use criterion::{Criterion, criterion_group, criterion_main};
use hw45::{xor8sum, fletcher8sum, crc16sum};

fn openssl_rand(size_exp: u32) -> String {
    let outpath = format!("target/rand_{}", size_exp);
    if !Path::new(&outpath).exists() {
        Command::new("openssl")
            .args(["rand", "-out", &outpath, &2u32.pow(size_exp).to_string()])
            .status().unwrap();
    }
    return outpath;
}

fn checksum_rand(c: &mut Criterion) {
    for size_exp in (2..=20).step_by(2) {
        // initialize and open target/rand_* test files
        let filepath = openssl_rand(size_exp);
        let data = std::fs::read(&filepath).unwrap();

        // benchmark checksum functions
        c.bench_function( &format!("xor8sum {}", &filepath),
                          |b| b.iter(|| xor8sum(&data)) );
        c.bench_function( &format!("fletcher8sum {}", &filepath),
                          |b| b.iter(|| fletcher8sum(&data)) );
        c.bench_function( &format!("crc16sum {}", &filepath),
                          |b| b.iter(|| crc16sum(&data)) );
    }
}

criterion_group!(benches, checksum_rand);
criterion_main!(benches);
