
use std::fs::*;
use std::io::prelude::*;

enum ConstPool {

}

fn main() -> std::io::Result<()> {
    println!("Hello, world!");
    let mut file = File::open("test/Test.java")?;
    // let mut file = File::open("test/Test.class")?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    println!("Hello, world!");
    println!("{}", contents);
    Ok(())
}
