// use std::io::*;
use std::fs;
// use std::fs::*;

use bytes::Buf;
use bytes::Bytes;

use std::process::exit;
use std::io::{BufRead, Read};
use std::str::from_utf8;

mod ConstPoolTag {
    /// Represents the null entry which cannot be addressed.
    /// This is used for the NULLENTRY (position zero) and
    /// for long and double entries.
    pub const NULLENTRY: u8 = 0;

    /// Represents a class or an interface.
    pub const CLASS: u8 = 7;

    /// Represents a field.
    pub const FIELDREF: u8 = 9;

    /// Represents a method.
    pub const METHODREF: u8 = 10;

    /// Represents an inteface method.
    pub const INTERMETHODREF: u8 = 11;

    /// Used to represent constant objects of the type String.
    pub const STRING: u8 = 8;

    /// Represents 4-byte numeric int constants.
    pub const INTEGER: u8 = 3;

    /// Represents 4-byte numeric float constants.
    pub const FLOAT: u8 = 4;

    /// Represents 8-byte numeric long constants.
    pub const LONG: u8 = 5;

    /// Represents 8-byte numeric double constants.
    pub const DOUBLE: u8 = 6;

    /// Used to represent a field or method, without indicating which class
    /// or interface type it belongs to.
    pub const NAMEANDTYPE: u8 = 12;

    /// Used to represent constant string values.
    pub const UTF8: u8 = 1;

    /// Used to represent a method handle.
    pub const METHODHANDLE: u8 = 15;

    /// Used to represent a method type.
    pub const METHODTYPE: u8 = 16;

    /// Used by an invokedynamic instruction to specify a bootstrap method,
    /// the dynamic invocation name, the argument and return types of the
    /// call, and optionally, a sequence of additional constants called
    /// static arguments to the bootstrap method.
    pub const INVOKEDYNAMIC: u8 = 18;
}

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

    for mut i in 1..count {
        let tag = c.get_u8();
        match tag {
            ConstPoolTag::CLASS => {
                let classNameIndex = c.get_u16();
                println!("CP Class");
            }
            ConstPoolTag::FIELDREF => {
                let classIndex = c.get_u16();
                let nameAndTypeIndex = c.get_u16();
                println!("CP Field");
            }
            ConstPoolTag::METHODREF => {
                let classIndex = c.get_u16();
                let nameAndTypeIndex = c.get_u16();
                println!("CP Method");
            }
            ConstPoolTag::INTERMETHODREF => {
                let classIndex = c.get_u16();
                let nameAndTypeIndex = c.get_u16();
                println!("CP Interface");
            }
            ConstPoolTag::STRING => {
                let utf8Index = c.get_u16();
                println!("String");
            }
            ConstPoolTag::INTEGER => {
                let value = c.get_u32();
                // cp -> addInteger(value);
            }
            ConstPoolTag::FLOAT => {
                let value = c.get_u32();
                // float fvalue = *(float *) & value;
                // cp -> addFloat(fvalue);
            }
            ConstPoolTag::LONG => {
                let high = c.get_u32();
                let low = c.get_u32();
                // long value = ((long) high << 32) + low;
                // cp -> addLong(value);
                i += 1;
            }
            ConstPoolTag::DOUBLE => {
                let high = c.get_u32();
                let low = c.get_u32();
                // long lvalue = ((long) high << 32) + low;
                // double dvalue = *(double *) &lvalue;
                // cp->addDouble(dvalue);
                i += 1;
            }
            ConstPoolTag::NAMEANDTYPE => {
                let nameIndex = c.get_u16();
                let descIndex = c.get_u16();
                // cp -> addNameAndType(nameIndex, descIndex);
            }
            ConstPoolTag::UTF8 => {
                let len = c.get_u16() as usize;
                // cp -> addUtf8((const char *) c.pos(), len);
                // let bs= c.to_bytes();
                let mut arr = vec![0 as u8; len];
                c.copy_to_slice(arr.as_mut_slice());
                // let mut utfbuf: &[u8]= &bs.slice(0..len);
                let mut utfbuf: &[u8]= arr.as_slice();
                print!("UTF-8: {:?}", from_utf8( utfbuf));
            }
            ConstPoolTag::METHODHANDLE => {
                let refKind = c.get_u8();
                let refIndex = c.get_u16();
                // cp -> addMethodHandle(refKind, refIndex);
            }
            ConstPoolTag::METHODTYPE => {
                let descIndex = c.get_u16();
                // cp -> addMethodType(descIndex);
            }
            ConstPoolTag::INVOKEDYNAMIC => {
                let bootMethodAttrIndex = c.get_u16();
                let nameAndTypeIndex = c.get_u16();
                // cp -> addInvokeDynamic(bootMethodAttrIndex, nameAndTypeIndex);
            }
            _ => {
                panic!("invalid tag");
            }
        }
    }

    println!("Magic: {:x}", magic);
    println!("major.minor: {}.{}", major, minor);
    println!("CP count: {}", count);

    println!("Magic: {:x} {:x} {:x} {:x}", data[0], data[1], data[2], data[3]);
    Ok(())
}
