use std::io::prelude::*;
use std::net::{TcpListener, TcpStream};

// ref: https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html
fn main() {
    let listener = TcpListener::bind("127.0.0.1:0").unwrap();
    println!("Server: http://{}/", listener.local_addr().unwrap());

    for stream in listener.incoming() {
        handle_http_request(stream.unwrap());
    }
}

fn handle_http_request(mut stream: TcpStream) {
    let mut request_buffer = [0u8; 1024];
    stream.read(&mut request_buffer).unwrap(); // TODO handle error
    let request_buffer = String::from_utf8_lossy(&request_buffer);
    println!("{}", request_buffer);

    // TODO handle arbitrary GET requests
    let response = if request_buffer.starts_with("GET / HTTP/1.1\r\n") {
        get_page(&"public/index.html".to_string(), 200)
    } else {
        get_page(&"public/404.html".to_string(), 404)
    };

    println!("{}", response);
    stream.write(response.as_bytes()).unwrap(); // TODO handle error
    stream.flush().unwrap(); // TODO handle error
}

fn get_page(filepath: &String, status: u32) -> String {
    let body = std::fs::read_to_string(filepath).unwrap(); // TODO handle error
    return get_response(status, &body);
}

fn get_response(status: u32, body: &String) -> String {
    return format!(
        "HTTP/1.1 {} _\r\nContent-Length: {}\r\n\r\n{}",
        status, body.len(), body
    );
}
