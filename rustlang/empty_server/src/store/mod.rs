use std::io::{Read};

pub use self::store_s3::StoreS3 as StoreS3;
use std::error::Error;

mod store_s3;

pub trait Store {

    fn put(&self, object_name: &str, v: Vec<u8>) -> bool;

    fn get(&self, object_name: &str) -> Result<Vec<u8>, ()>;
}
