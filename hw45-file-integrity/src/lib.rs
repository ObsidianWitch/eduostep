pub fn xor8sum<'a>(iterable: impl IntoIterator<Item = &'a u8>) -> u8 {
    return iterable.into_iter().fold(0u8, |acc, b| acc ^ b);
}

pub fn fletcher8sum<'a>(iterable: impl IntoIterator<Item = &'a u8>) -> (u8, u8) {
    return iterable.into_iter().fold((0u8, 0u8), |acc, b| {
        let s1 = (acc.0 as u16 + *b as u16) % 255;
        let s2 = (acc.1 as u16 + s1) % 255;
        return (s1 as u8, s2 as u8);
    });
}

// ref: http://ross.net/crc/download/crc_v3.txt sections [1;8]
// ref: http://boston.conman.org/2014/10/26.1
// ref: https://en.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks
// ref: http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
// parameters: CRC16_CCIT_ZERO
pub fn crc16sum<'a>(iterable: impl IntoIterator<Item = &'a u8>) -> u16 {
    return iterable.into_iter().fold(0u16, |mut crc, b| {
        crc ^= (*b as u16) << 8;
        for _ in 0..8 {
            crc = if (crc & 0x8000u16) != 0 { (crc << 1) ^ 0x1021u16 }
                                       else { crc << 1 };
        }
        return crc;
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
        assert!(crc16sum(&data) == 0xda91);
    }
}
