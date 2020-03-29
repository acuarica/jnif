// use std::io::*;
use std::fs;
// use std::fs::*;

use bytes::Buf;

// enum ConstPool {
// }

fn main() -> std::io::Result<()> {
    // let mut file = File::open("test/Main.class")?;
    // let mut data = Vec::new();
    let data = fs::read("test/Main.class")?;
    // file.read_to_end(&mut data);
    let mut c: &[u8] = &data;
    let magic = c.get_u32();
    assert_eq!(0xcafebabe, magic);

    let minor = c.get_u16();
    let major = c.get_u16();

    let count = c.get_u16();

    println!("Magic: {:x}", magic);
    println!("major.minor: {}.{}", major, minor);
    println!("CP count: {}", count);

    println!("Magic: {:x} {:x} {:x} {:x}", data[0], data[1], data[2], data[3]);
    Ok(())
}