use std::net::{TcpListener, TcpStream};
use std::io::{BufRead, BufReader, Write};
use std::time::Duration;

// ref: https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html
fn main() {
    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    println!("Server: http://{}/", listener.local_addr().unwrap());

    for stream in listener.incoming() {
        handle_http_request(stream.unwrap());
    }
}

fn handle_http_request(mut stream: TcpStream) {
    stream.set_read_timeout(Some(Duration::new(30, 0))).unwrap();
    stream.set_write_timeout(Some(Duration::new(30, 0))).unwrap();

    let mut reader = BufReader::new(&stream);
    let mut status_line = String::new();
    reader.read_line(&mut status_line).unwrap(); // TODO handle error
    println!("{}", status_line);

    // TODO handle arbitrary GET requests
    let response = if status_line.starts_with("GET / HTTP/1.1\r\n") {
        get_page(&"public/index.html".to_string(), 200)
    } else {
        get_page(&"public/404.html".to_string(), 404)
    };
    println!("{}", response);
    println!("---");
    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}

fn get_page(filepath: &String, status: u32) -> String {
    let body = std::fs::read_to_string(filepath).unwrap(); // TODO handle error
    return new_response(status, &body);
}

fn new_response(status: u32, body: &String) -> String {
    return format!(
        "HTTP/1.1 {} _\r\nContent-Length: {}\r\n\r\n{}",
        status, body.len(), body
    );
}
