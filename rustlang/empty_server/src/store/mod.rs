pub use self::store_s3::StoreS3 as StoreS3;
use std::io::Read;

mod store_s3;

pub trait Store {
    fn put<E: Read + Sized>(&self, object_name: &str, r: E) -> bool;
}
