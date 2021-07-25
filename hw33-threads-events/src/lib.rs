use std::path::PathBuf;

pub fn get_error_page(status: u32, reason: &str) -> String {
    let body = format!(
        "<!DOCTYPE html>
        <html lang=\"en\">
        <head><meta charset=\"utf-8\"><title>{0} {1}</title></head>
        <body><h1>{0} {1}</h1></body>
        </html>", status, reason
    );
    return new_response(status, reason, &body);
}

pub fn check_path(filepath: &str) -> std::io::Result<PathBuf> {
    let filepath = std::fs::canonicalize(filepath)?;
    let basepath = std::fs::canonicalize("public")?;
    if !filepath.starts_with(basepath) {
        return Err(std::io::Error::new(
            std::io::ErrorKind::PermissionDenied,
            "Cannot access files outside public/"
        ));
    }
    return Ok(filepath);
}

pub fn new_response(status: u32, reason: &str, body: &str) -> String {
    return format!(
        "HTTP/1.1 {} {}\r\nContent-Length: {}\r\n\r\n{}",
        status, reason, body.len(), body
    );
}
