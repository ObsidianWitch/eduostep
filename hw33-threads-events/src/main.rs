use std::net::{TcpListener, TcpStream};

// ref: https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html
fn main() {
    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    println!("TCP listener on {}", listener.local_addr().unwrap());

    for _stream in listener.incoming() {
        let stream = stream.unwrap();
        println!("Connection established.");
    }
}
