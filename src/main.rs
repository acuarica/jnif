use std::fs;

fn main() -> std::io::Result<()> {
    let data = fs::read("tests/Main.class")?;
    let res = jnif::parse(&data);

    if let Ok(cf) = res {
        println!("Magic: {:x}", jnif::ClassFile::MAGIC);
        println!("major.minor: {}.{}", cf.major, cf.minor);
        println!("CP count: {}", cf.cp.len());
        for e in cf.cp {
            println!("Entry: {}", e);
        }
        println!("flags: {:x}", cf.access_flags);
        println!("this: {}", cf.this_class_idx);
        println!("super: {}", cf.super_class_idx);
        println!("inter: {:?}", cf.interfaces);
        println!("f: {:?}", cf.fields);
        println!("m: {:?}", cf.methods);
    }

    Ok(())
}
