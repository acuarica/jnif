use std::io::*;
use std::fs::*;

enum ConstPool {
}

fn main() -> std::io::Result<()> {
    let mut file = File::open("test/Main.class")?;
    let mut data = Vec::new();
    file.read_to_end(&mut data);
    println!("Magic: {:x} {:x} {:x} {:x}", data[0], data[1], data[2], data[3]);
    Ok(())
}