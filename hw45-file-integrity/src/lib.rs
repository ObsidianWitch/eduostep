use std::io::{prelude::*, BufReader};
use std::fs::File;

pub fn xor8sum(path: &str) -> u8 {
    let file = File::open(path).unwrap();
    let file = BufReader::new(file);
    return file.bytes().fold(0u8, |acc, b| acc ^ b.unwrap());
}

pub fn fletcher8sum(path: &str) -> (u8, u8) {
    let file = File::open(path).unwrap();
    let file = BufReader::new(file);
    return file.bytes().fold((0u8, 0u8), |acc, b| {
        let s1 = (acc.0 as u16 + b.unwrap() as u16) % 255;
        let s2 = (acc.1 as u16 + s1) % 255;
        return (s1 as u8, s2 as u8);
    });
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn checksum_manual() {
        assert!(xor8sum("manual.md") == 3u8);
        assert!(fletcher8sum("manual.md") == (26u8,183u8));
    }
}
