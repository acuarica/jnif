
use std::io::*;
use std::fs::*;
use std::io::prelude::*;


enum ConstPool {

}

fn solution() -> bool {


}

fn main2() -> std::io::Result<()> {
    println!("Hello, world!");
    // let mut file = File::open("test/Test.java")?;
    let mut file = File::open("test/Test.class")?;
    let mut data = Vec::new();
    file.read_to_end(&mut data);
    println!("{}", data[0]);
    println!("{}", data[1]);
    println!("{}", data[2]);
    println!("{}", data[3]);

    println!("Hello, world!");
    // println!("{}", contents);
    Ok(())
}


fn main() -> std::io::Result<()> {
    println!("Hello, world!");
    // let mut file = File::open("test/Test.java")?;
    let mut file = File::open("test/Test.class")?;
    let mut data = Vec::new();
    file.read_to_end(&mut data);
    println!("{}", data[0]);
    println!("{}", data[1]);
    println!("{}", data[2]);
    println!("{}", data[3]);

    println!("Hello, world!");
    // println!("{}", contents);
    Ok(())
}
