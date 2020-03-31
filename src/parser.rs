use bytes::Buf;
use std::str::from_utf8;
use std::result;

/// The Index type represents how each item within the constant pool can be addressed.
/// The specification indicates that this is an u16 value.
type Idx = u16;

// struct ConstPool {

// }

pub mod const_pool_tag {
    /// Represents the null entry which cannot be addressed.
    /// This is used for the NULLENTRY (position zero) and
    /// for long and double entries.
    // pub const NULL_ENTRY: u8 = 0;

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

    /// Used by an `invokedynamic` instruction to specify a bootstrap method,
    /// the dynamic invocation name, the argument and return types of the
    /// call, and optionally, a sequence of additional constants called
    /// static arguments to the bootstrap method.
    pub const INVOKE_DYNAMIC: u8 = 18;
}

#[derive(Debug)]
#[repr(u8)]
pub enum Entry {
    Class { name_idx: Idx },
    FieldRef { class_idx: Idx, name_and_type_idx: Idx },
    MethodRef { class_idx: Idx, name_and_type_idx: Idx },
    InterfaceRef { class_idx: Idx, name_and_type_idx: Idx },
}

pub struct ClassFile {
    pub minor: u16,
    pub major: u16,
}

impl ClassFile {
    const MAGIC: u32 = 0xcafebabe;
}

pub fn parse(data: &Vec<u8>) -> result::Result<ClassFile, &str> {
    let mut c: &[u8] = &data;
    let magic = c.get_u32();
    if magic != ClassFile::MAGIC {
        return Err("Invalid magic header");
    }

    let minor = c.get_u16();
    let major = c.get_u16();

    let count = c.get_u16();

    let mut i = 1;
    while i < count {
        let tag = c.get_u8();
        print!("index: {}, ", i);
        match tag {
            const_pool_tag::CLASS => {
                let name_idx = c.get_u16();
                let entry = Entry::Class { name_idx };
                println!("Class: {:?}", entry);
            }
            const_pool_tag::FIELDREF => {
                let class_idx = c.get_u16();
                let name_and_type_idx = c.get_u16();
                let entry = Entry::FieldRef { class_idx, name_and_type_idx };
                println!("CP Field: {:?}", entry);
            }
            const_pool_tag::METHODREF => {
                let class_idx = c.get_u16();
                let name_and_type_idx = c.get_u16();
                let entry = Entry::MethodRef { class_idx, name_and_type_idx };
                println!("CP Method: {:?}", entry);
            }
            const_pool_tag::INTERMETHODREF => {
                let class_idx = c.get_u16();
                let name_and_type_idx = c.get_u16();
                let entry = Entry::InterfaceRef { class_idx, name_and_type_idx };
                println!("CP Interface: {:?}", entry);
            }
            const_pool_tag::STRING => {
                let utf8_idx = c.get_u16();
                println!("String idx: {}", utf8_idx);
            }
            const_pool_tag::INTEGER => {
                let val = c.get_u32();
                println!("integer val: {}", val);
            }
            const_pool_tag::FLOAT => {
                let val = c.get_f32();
                // float fvalue = *(float *) & value;
                // cp -> addFloat(fvalue);
                println!("float val: {}", val);
            }
            const_pool_tag::LONG => {
                let val = c.get_i64();
                // long value = ((long) high << 32) + low;
                // cp -> addLong(value);
                println!("long val: {}", val);
                i += 1;
            }
            const_pool_tag::DOUBLE => {
                // let _high = c.get_u32();
                // let _low = c.get_u32();
                let val = c.get_f64();
                println!("double val: {}", val);
                // long lvalue = ((long) high << 32) + low;
                // double dvalue = *(double *) &lvalue;
                // cp->addDouble(dvalue);
                i += 1;
            }
            const_pool_tag::NAMEANDTYPE => {
                let _name_index = c.get_u16();
                let _desc_index = c.get_u16();
                // cp -> addNameAndType(nameIndex, descIndex);
            }
            const_pool_tag::UTF8 => {
                let len = c.get_u16() as usize;
                // cp -> addUtf8((const char *) c.pos(), len);
                // let bs= c.to_bytes();
                let mut arr = vec![0 as u8; len];
                c.copy_to_slice(arr.as_mut_slice());
                // let mut utfbuf: &[u8]= &bs.slice(0..len);
                let utfbuf: &[u8] = arr.as_slice();
                println!("UTF-8: {:?}", from_utf8(utfbuf));
            }
            const_pool_tag::METHODHANDLE => {
                let _ref_kind = c.get_u8();
                let _ref_index = c.get_u16();
                // cp -> addMethodHandle(refKind, refIndex);
            }
            const_pool_tag::METHODTYPE => {
                let _desc_index = c.get_u16();
                // cp -> addMethodType(descIndex);
            }
            const_pool_tag::INVOKE_DYNAMIC => {
                let _boot_method_attr_index = c.get_u16();
                let _name_and_type_index = c.get_u16();
                // cp -> addInvokeDynamic(bootMethodAttrIndex, nameAndTypeIndex);
            }
            _ => {
                panic!("invalid tag");
            }
        }

        i += 1;
    }

    let cf = ClassFile { major, minor };

    println!("Magic: {:x}", magic);
    println!("major.minor: {}.{}", major, minor);
    println!("CP count: {}", count);

    Ok(cf)
}
