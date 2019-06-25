
use std::fs::*;
use std::io::prelude::*;

enum ConstPool {

}

fn main() -> std::io::Result<()> {
    // let mut file = File::open("hola.txt")?;
    let mut file = File::open("test/Test.class")?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    println!("Hello, world!");
    println!("{}", contents);
    Ok(())
}
