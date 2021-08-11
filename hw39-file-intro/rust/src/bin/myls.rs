use std::path::Path;
use std::os::linux::fs::MetadataExt;

fn print_one(path: &Path, verbose: bool) {
    if verbose {
        let md = path.symlink_metadata().unwrap();
        println!("{:o} {} {} {} {} {} {:?}", md.st_mode(), md.st_nlink(),
            md.st_uid(), md.st_gid(), md.st_size(), md.st_mtime(), path);
    } else {
        println!("{:?} ", path);
    }
}

// Q2. List Files: Write a program that lists files in the given directory. When
// called without any arguments, the program should just print the file names.
// When invoked with the -l flag, the program should print out information about
// each file, such as the owner, group, per- missions, and other information
// obtained from the stat() system call. The program should take one additional
// argument, which is the directory to read, e.g., myls -l directory. If no
// directory is given, the program should just use the current working
// directory. Useful interfaces: stat(), opendir(), readdir(), getcwd().
fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut options = getopts::Options::new();
    options.optflag("h", "help", "display help");
    options.optflag("l", "long", "long listing format");
    let matches = options.parse(&args[1..]).unwrap();

    if matches.opt_present("h") {
        println!("{}", options.usage("usage: myls [option]... path"));
        return;
    }

    let verbose = matches.opt_present("l");
    let path = if matches.free.is_empty() { ".".to_string() }
               else { matches.free[0].clone() };
    let path = Path::new(&path);
    if !path.exists() {
        panic!("cannot access {:?}: No such file or directory", path);
    }

    if !path.is_dir() {
        print_one(path, verbose);
    } else {
        for entry in path.read_dir().unwrap() {
            let entry = entry.unwrap();
            print_one(&entry.path(), verbose);
        }
    }
}
