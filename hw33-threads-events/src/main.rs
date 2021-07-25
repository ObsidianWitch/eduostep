use std::io::{BufRead, BufReader, Write};
use std::net::{TcpListener, TcpStream};
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
    let response = if let Err(_) = reader.read_line(&mut status_line) {
        get_error_page(500, "Internal Server Error")
    } else {
        println!("{}", status_line);
        let status_line: Vec<&str> = status_line.split(' ').collect();
        if status_line.get(0) != Some(&"GET") {
            get_error_page(501, "Not Implemented")
        } else if status_line.get(2) != Some(&"HTTP/1.1\r\n") {
            get_error_page(400, "Bad Request")
        } else if let Some(path) = status_line.get(1) {
            get_page(path).unwrap_or_else(|_|
                get_error_page(404, "Not Found")
            )
        } else {
            get_error_page(400, "Bad Request")
        }
    };
    println!("{}\n---", response);
    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}

fn get_error_page<Str: AsRef<str>>(status: u32, reason: Str) -> String {
    let reason = reason.as_ref();
    let body = format!(
        "<!DOCTYPE html>
        <html lang=\"en\">
        <head><meta charset=\"utf-8\"><title>{0} {1}</title></head>
        <body><h1>{0} {1}</h1></body>
        </html>", status, reason
    );
    return new_response(status, reason, &body);
}

fn get_page<Str: AsRef<str>>(filepath: Str) -> Result<String, std::io::Error> {
    let filepath = format!("public/{}", filepath.as_ref());
    // TODO check absolute(filepath) is contained in absolute("public/")
    let body = std::fs::read_to_string(&filepath)?;
    return Ok(new_response(200, "Ok", &body));
}

fn new_response<Str: AsRef<str>>(status: u32, reason: Str, body: Str) -> String {
    let reason = reason.as_ref();
    let body = body.as_ref();
    return format!(
        "HTTP/1.1 {} {}\r\nContent-Length: {}\r\n\r\n{}",
        status, reason, body.len(), body
    );
}
