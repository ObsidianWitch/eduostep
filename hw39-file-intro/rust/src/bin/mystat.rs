use std::path::Path;
use std::os::linux::fs::MetadataExt;

// Q1. Stat: Write your own version of the command line program stat, which
// simply calls the stat() system call on a given file or directory. Print out
// file size, number of blocks allocated, reference (link) count, and so forth.
// What is the link count of a directory, as the number of entries in the
// directory changes? Useful interfaces: stat().
fn main() {
    let args: Vec<String> = std::env::args().collect();
    let path = args.get(1).expect("missing operand: path");
    let path = Path::new(path);
    if !path.exists() {
        panic!("cannot access {:?}: No such file or directory", path);
    }
    let md = path.symlink_metadata().unwrap();
    println!("File: {:?}\nSize: {}\nBlocks: {}\nLinks: {}\n...",
        path, md.st_size(), md.st_blocks(), md.st_nlink());
}
