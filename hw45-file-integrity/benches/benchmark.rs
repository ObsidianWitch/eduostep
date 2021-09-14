use std::io::{prelude::*, BufReader};
use std::fs::File;
use std::path::Path;
use std::process::Command;
use criterion::{Criterion, criterion_group, criterion_main};
use hw45::{xor8sum, fletcher8sum};

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
    for size_exp in (10..=30).step_by(5) {
        // initialize target/rand_* test files
        let filepath = openssl_rand(size_exp);
        let file = File::open(&filepath).unwrap();
        let mut reader = BufReader::new(file);

        // benchmark checksum functions
        c.bench_function( &format!("xor8sum {}", &filepath),
                          |b| b.iter(|| xor8sum(&mut reader)) );
        reader.rewind().unwrap();
        c.bench_function( &format!("fletcher8sum {}", &filepath),
                          |b| b.iter(|| fletcher8sum(&mut reader)) );
    }
}

criterion_group!(benches, checksum_rand);
criterion_main!(benches);
