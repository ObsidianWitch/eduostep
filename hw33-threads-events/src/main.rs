use std::io::prelude::*;
use std::net::{TcpListener, TcpStream};

// ref: https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html
fn main() {
    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    println!("TCP listener on {}", listener.local_addr().unwrap());

    for stream in listener.incoming() {
        handle_connection(stream.unwrap());
    }
}

fn handle_connection(mut stream: TcpStream) {
    let mut buffer = [0; 1024]; // cannot handle arbitrary size requests.
    stream.read(&mut buffer).unwrap();
    println!("{}", String::from_utf8_lossy(&buffer));

    let file_string = std::fs::read_to_string("public/index.html").unwrap();
    let response = format!(
        "HTTP/1.1 200 OK\r\n \
        Content-Length: {}\r\n\r\n \
        {}", file_string.len(), file_string
    );
    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}
