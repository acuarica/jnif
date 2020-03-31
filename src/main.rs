use std::fs;

fn main() -> std::io::Result<()> {
    let data = fs::read("tests/Main.class")?;
    let res = jnif::parse(&data);

    if let Ok(cf) = res {
        assert_eq!(55, cf.major);
        assert_eq!(0, cf.minor);
        println!("Magic: {:x}", jnif::ClassFile::MAGIC);
        println!("major.minor: {}.{}", cf.major, cf.minor);
        println!("CP count: {}", cf.cp.len());

        for e in cf.cp {
            println!("Entry: {}", e);
        }
    }

    Ok(())
}
