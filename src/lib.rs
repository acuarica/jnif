#![feature(arbitrary_enum_discriminant)]

use bytes::Buf;
use std::str::from_utf8;
use std::result;
use std::fmt;

/// The Index type represents how each item within the constant pool can be addressed.
/// The specification indicates that this is an u16 value.
type Idx = u16;

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
    Empty,

    /// Represents a class, interface or array type.
    /// The name of the class of this type.
    Class { name_idx: Idx },
    FieldRef { class_idx: Idx, name_n_type_idx: Idx },
    MethodRef { class_idx: Idx, name_n_type_idx: Idx },
    InterfaceRef { class_idx: Idx, name_n_type_idx: Idx },

    /// Represents a String constant value.
    /// The index of the utf8 entry containing this string value.
    /// See [`Entry::Class`].
    String { utf8_idx: Idx },

    /// Represents an integer constant value.
    /// The integer value of this entry.
    Integer { val: i32 },

    /// Represent a float constant value.
    /// The float value of this entry.
    Float { val: f32 },

    /// Represents a long constant value.
    /// The long value of this entry.
    Long { val: i64 },

    /// Represents a double constant value.
    /// The double value of this entry.
    Double { val: f64 },

    /// Represents a tuple of name and descriptor.
    /// The index of the utf8 entry containing its name.
    /// The utf8 entry index containing its descriptor.
    NameAndType { name_idx: Idx, desc_idx: Idx },

    /// Contains a modified UTF-8 string.
    /// The string data.
    Utf8 { utf8_buf: Vec<u8> },

    /// Represents a method handle entry.
    /// The reference kind of this entry.
    /// The reference index of this entry.
    MethodHandle { ref_kind: u8, ref_idx: u16 },

    /// Represents the type of a method.
    /// The utf8 index containing the descriptor of this entry.
    MethodType { desc_idx: Idx },

    /// Represents an invoke dynamic call site.
    /// The bootstrap method attribute index.
    /// The name and type index of this entry.
    /// @see ConstNameAndType
    InvokeDynamic { bootstrap_method_attr_idx: Idx, name_n_type_idx: Idx },
}

impl fmt::Display for Entry {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match &*self {
            Entry::Utf8 { utf8_buf } => {
                let xs: &[u8] = utf8_buf.as_slice();
                write!(f, "Utf8 entry: `{:?}'", from_utf8(xs))
            }
            _ => fmt::Debug::fmt(self, f)
        }
    }
}

#[derive(Debug)]
pub struct ClassFile {
    pub minor: u16,
    pub major: u16,
    pub cp: ConstPool,
    pub access_flags: u16,
    pub this_class_idx: Idx,
    pub super_class_idx: Idx,
    pub interfaces: Vec<Idx>,
    pub fields: Vec<Member>,
    pub methods: Vec<Member>,
}

type ConstPool = Vec<Entry>;

impl ClassFile {
    pub const MAGIC: u32 = 0xcafebabe;
}

#[derive(Debug)]
pub struct Member {
    pub access_flags: u16,
    pub name_idx: Idx,
    pub desc_idx: Idx,
}

pub fn parse_cp<TBuf: Buf>(c: &mut TBuf, cp: &mut ConstPool) {
    let count = c.get_u16();

    let mut i = 1;
    while i < count {
        let tag = c.get_u8();

        match tag {
            const_pool_tag::CLASS => {
                let name_idx = c.get_u16();
                cp.push(Entry::Class { name_idx });
            }
            const_pool_tag::FIELDREF => {
                let class_idx = c.get_u16();
                let name_n_type_idx = c.get_u16();
                cp.push(Entry::FieldRef { class_idx, name_n_type_idx });
            }
            const_pool_tag::METHODREF => {
                let class_idx = c.get_u16();
                let name_n_type_idx = c.get_u16();
                cp.push(Entry::MethodRef { class_idx, name_n_type_idx });
            }
            const_pool_tag::INTERMETHODREF => {
                let class_idx = c.get_u16();
                let name_n_type_idx = c.get_u16();
                cp.push(Entry::InterfaceRef { class_idx, name_n_type_idx });
            }
            const_pool_tag::STRING => {
                let utf8_idx = c.get_u16();
                cp.push(Entry::String { utf8_idx });
            }
            const_pool_tag::INTEGER => {
                let val = c.get_i32();
                cp.push(Entry::Integer { val });
            }
            const_pool_tag::FLOAT => {
                let val = c.get_f32();
                cp.push(Entry::Float { val });
            }
            const_pool_tag::LONG => {
                let val = c.get_i64();
                cp.push(Entry::Long { val });
                i += 1;
            }
            const_pool_tag::DOUBLE => {
                let val = c.get_f64();
                cp.push(Entry::Double { val });
                i += 1;
            }
            const_pool_tag::NAMEANDTYPE => {
                let name_idx = c.get_u16();
                let desc_idx = c.get_u16();
                cp.push(Entry::NameAndType { name_idx, desc_idx });
            }
            const_pool_tag::UTF8 => {
                let len = c.get_u16() as usize;
                let mut arr = vec![0 as u8; len];
                c.copy_to_slice(arr.as_mut_slice());
                cp.push(Entry::Utf8 { utf8_buf: arr });
            }
            const_pool_tag::METHODHANDLE => {
                let ref_kind = c.get_u8();
                let ref_idx = c.get_u16();
                cp.push(Entry::MethodHandle { ref_kind, ref_idx });
            }
            const_pool_tag::METHODTYPE => {
                let desc_idx = c.get_u16();
                cp.push(Entry::MethodType { desc_idx });
            }
            const_pool_tag::INVOKE_DYNAMIC => {
                let bootstrap_method_attr_idx = c.get_u16();
                let name_n_type_idx = c.get_u16();
                cp.push(Entry::InvokeDynamic { bootstrap_method_attr_idx, name_n_type_idx });
            }
            _ => {
                panic!("invalid tag");
            }
        }

        i += 1;
    }
}

pub fn parse(data: &Vec<u8>) -> result::Result<ClassFile, &str> {
    let mut c: &[u8] = &data;
    let magic = c.get_u32();

    if magic != ClassFile::MAGIC {
        return Err("Invalid magic header");
    }

    let minor = c.get_u16();
    let major = c.get_u16();
    let mut cf = ClassFile {
        major,
        minor,
        cp: vec![],
        access_flags: 0,
        this_class_idx: 0,
        super_class_idx: 0,
        interfaces: vec![],
        fields: vec![],
        methods: vec![],
    };

    parse_cp(&mut c, &mut cf.cp);

    cf.access_flags = c.get_u16();
    cf.this_class_idx = c.get_u16();
    cf.super_class_idx = c.get_u16();

    let inter_count = c.get_u16();
    for _i in 0..inter_count {
        let inter_index = c.get_u16();
        cf.interfaces.push(inter_index);
    }

    let field_count = c.get_u16();
    for _i in 0..field_count {
        let access_flags = c.get_u16();
        let name_idx = c.get_u16();
        let desc_idx = c.get_u16();
        cf.fields.push( Member { access_flags, name_idx, desc_idx });
        // FieldAttrsParser().parse(br, cf, &f.attrs);
    }

    let method_count = c.get_u16();
    for _i in 0..method_count {
        let access_flags = c.get_u16();
        let name_idx = c.get_u16();
        let desc_idx = c.get_u16();
        cf.methods.push( Member { access_flags, name_idx, desc_idx });
        // MethodAttrsParser().parse(br, cf, &m.attrs);
    }

    // ClassAttrsParser().parse(br, cf, &cp.attrs);

    Ok(cf)
}
