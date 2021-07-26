use std::io::{prelude::*, BufReader};
use std::net::{TcpListener, TcpStream};
use std::time::Duration;
use hw33::*;

// ref: https://doc.rust-lang.org/book/ch20-00-final-project-a-web-server.html
fn main() -> std::io::Result<()> {
    let listener = TcpListener::bind("127.0.0.1:7878")?;
    println!("Server: http://{}/", listener.local_addr()?);

    for stream in listener.incoming() {
        handle_http_request(stream?);
    }
    return Ok(());
}

fn handle_http_request(mut stream: TcpStream) {
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
            let path = route(path);
            get_page(path).unwrap_or_else(|_| get_error_page(404, "Not Found"))
        } else {
            get_error_page(400, "Bad Request")
        }
    };
    println!("{}\n---", response);
    stream.write(response.as_bytes()).unwrap();
    stream.flush().unwrap();
}

fn route(path: &str) -> &str {
    return match path {
        "/" => "/index.html",
        "/sleep" => {
            std::thread::sleep(Duration::from_millis(100));
            "/index.html"
        },
        path => path,
    };
}

fn get_page(filepath: &str) -> std::io::Result<String> {
    let filepath = format!("public/{}", filepath);
    let filepath = check_path(&filepath)?;
    let body = std::fs::read_to_string(&filepath)?;
    return Ok(new_response(200, "Ok", &body));
}
