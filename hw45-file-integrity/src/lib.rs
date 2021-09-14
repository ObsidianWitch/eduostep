use std::io::prelude::*;

pub fn xor8sum<R: Read>(reader: &mut R) -> u8 {
    return reader.bytes().fold(0u8, |acc, b| acc ^ b.unwrap());
}

pub fn fletcher8sum<R: Read>(reader: &mut R) -> (u8, u8) {
    return reader.bytes().fold((0u8, 0u8), |acc, b| {
        let s1 = (acc.0 as u16 + b.unwrap() as u16) % 255;
        let s2 = (acc.1 as u16 + s1) % 255;
        return (s1 as u8, s2 as u8);
    });
}

#[cfg(test)]
mod tests {
    use std::io::{prelude::*, BufReader};
    use std::fs::File;
    use super::*;

    #[test]
    fn checksum_manual() {
        let file = File::open("manual.md").unwrap();
        let mut reader = BufReader::new(file);
        assert!(xor8sum(&mut reader) == 3u8);

        reader.rewind().unwrap();
        assert!(fletcher8sum(&mut reader) == (26u8,183u8));
    }
}
