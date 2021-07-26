use async_std::prelude::*;
use async_std::io::BufReader;
use async_std::net::{TcpListener, TcpStream};
use futures::stream::StreamExt;
use std::time::Duration;
use hw33::*;

// ref: https://rust-lang.github.io/async-book/09_example/00_intro.html
// ref: https://docs.rs/async-std
#[async_std::main]
async fn main() -> std::io::Result<()> {
    let listener = TcpListener::bind("127.0.0.1:7878").await?;
    println!("Server: http://{}/", listener.local_addr()?);

    listener.incoming().for_each_concurrent(None, |stream| async move {
        handle_http_request(stream.unwrap()).await;
    }).await;
    return Ok(());
}

async fn handle_http_request(mut stream: TcpStream) {
    let mut reader = BufReader::new(&stream);
    let mut status_line = String::new();
    let response = if let Err(_) = reader.read_line(&mut status_line).await {
        get_error_page(500, "Internal Server Error")
    } else {
        println!("{}", status_line);
        let status_line: Vec<&str> = status_line.split(' ').collect();
        if status_line.get(0) != Some(&"GET") {
            get_error_page(501, "Not Implemented")
        } else if status_line.get(2) != Some(&"HTTP/1.1\r\n") {
            get_error_page(400, "Bad Request")
        } else if let Some(path) = status_line.get(1) {
            let path = route(path).await;
            get_page(path).await.unwrap_or_else(|_| get_error_page(404, "Not Found"))
        } else {
            get_error_page(400, "Bad Request")
        }
    };
    println!("{}\n---", response);
    stream.write(response.as_bytes()).await.unwrap();
    stream.flush().await.unwrap();
}

async fn route(path: &str) -> &str {
    return match path {
        "/" => "/index.html",
        "/sleep" => {
            async_std::task::sleep(Duration::from_millis(100)).await;
            "/index.html"
        },
        path => path,
    };
}

async fn get_page(filepath: &str) -> std::io::Result<String> {
    let filepath = format!("public/{}", filepath);
    let filepath = check_path(&filepath)?;
    let body = async_std::fs::read_to_string(&filepath).await?;
    return Ok(new_response(200, "Ok", &body));
}
