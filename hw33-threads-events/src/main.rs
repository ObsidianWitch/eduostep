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

    // TODO regex "GET \/.* HTTP\/1.1\r\n
    let response = if status_line.starts_with("GET") {
        let path = status_line.split(' ').nth(1).unwrap();
        get_page(path)
    } else {
        get_error_page(501, "Not Implemented")
    };
    println!("{}", response);
    println!("---");
    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}

fn get_error_page<Str: AsRef<str>>(status: u32, reason: Str) -> String {
    let reason = reason.as_ref();
    let body = format!("<!DOCTYPE html>
        <html lang=\"en\">
        <head><meta charset=\"utf-8\"><title>{0} {1}</title></head>
        <body><h1>{0} {1}</h1></body>
        </html>", status, reason);
    return new_response(status, reason, &body);
}

fn get_page<Str: AsRef<str>>(filepath: Str) -> String {
    let filepath = format!("public/{}", filepath.as_ref());
    // TODO check absolute(filepath) is contained in absolute("public/")
    return match std::fs::read_to_string(&filepath) {
        Ok(body) => new_response(200, "Ok", &body),
        Err(e) => {
            eprintln!("get_page({}): {}", &filepath, e);
            get_error_page(404, "Not Found")
        },
    }
}

fn new_response<Str: AsRef<str>>(status: u32, reason: Str, body: Str) -> String {
    let reason = reason.as_ref();
    let body = body.as_ref();
    return format!(
        "HTTP/1.1 {} {}\r\nContent-Length: {}\r\n\r\n{}",
        status, reason, body.len(), body
    );
}
