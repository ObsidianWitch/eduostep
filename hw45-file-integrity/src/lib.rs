pub fn xor8sum(data: &Vec<u8>) -> u8 {
    return data.iter().fold(0u8, |acc, b| acc ^ b);
}

pub fn fletcher8sum(data: &Vec<u8>) -> (u8, u8) {
    return data.iter().fold((0u8, 0u8), |acc, b| {
        let s1 = (acc.0 as u16 + *b as u16) % 255;
        let s2 = (acc.1 as u16 + s1) % 255;
        return (s1 as u8, s2 as u8);
    });
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn checksum_manual() {
        let data = std::fs::read("manual.md").unwrap();
        assert!(xor8sum(&data) == 3u8);
        assert!(fletcher8sum(&data) == (26u8,183u8));
    }
}
