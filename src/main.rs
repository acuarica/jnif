use std::fs;
use jnif::parser;

fn main() -> std::io::Result<()> {
    let data = fs::read("tests/Main.class")?;
    parser::parse(&data);

    Ok(())
}
