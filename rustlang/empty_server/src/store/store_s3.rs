#![cfg(feature = "s3")]
extern crate futures;
extern crate futures_fs;
extern crate rusoto_core;
extern crate rusoto_s3;

use futures::{Future, Stream};
use futures_fs::FsPool;
use rusoto_core::credential::{AwsCredentials, DefaultCredentialsProvider, StaticProvider};
use rusoto_core::{ProvideAwsCredentials, Region, RusotoError};
use rusoto_s3::util::{PreSignedRequest, PreSignedRequestOption};
use rusoto_s3::{
    CORSConfiguration, CORSRule, CompleteMultipartUploadRequest, CompletedMultipartUpload,
    CompletedPart, CopyObjectRequest, CreateBucketRequest, CreateMultipartUploadRequest,
    DeleteBucketRequest, DeleteObjectRequest, GetObjectError, GetObjectRequest, HeadObjectRequest,
    ListObjectsRequest, ListObjectsV2Request, PutBucketCorsRequest, PutObjectRequest, S3Client,
    StreamingBody, UploadPartCopyRequest, UploadPartRequest, S3,
};
use std::env;
use super::Store;
use std::io::Read;
use std::result::Result::Ok;

pub struct StoreS3 {
    region: Region,
    s3: S3Client,
    bucket_name: String,
    bucket_deleted: bool,
}

impl Store for StoreS3 {
    fn put<E: Read + Sized>(&self, object_name: &str, r: E) -> bool {
        let put_request = PutObjectRequest {
            bucket: self.bucket_name.to_owned(),
            key: object_name.to_owned(),
//            body: Some(r),
            ..Default::default()
        };
        if let Err(reason) = self.s3.put_object(put_request).sync() {
            error!("{:?}", reason);
            false
        } else {
            true
        }
    }
}

impl StoreS3 {
    pub fn new(bucket_name: String) -> Self {
        let region = if let Ok(endpoint) = env::var("S3_ENDPOINT") {
            let region = Region::Custom {
                name: "us-east-1".to_owned(),
                endpoint: endpoint.to_owned(),
            };
            println!(
                "picked up non-standard endpoint {:?} from S3_ENDPOINT env. variable",
                region
            );
            region
        } else {
            Region::UsEast1
        };

        StoreS3 {
            region: region.to_owned(),
            s3: S3Client::new(region),
            bucket_name: bucket_name.to_owned(),
            bucket_deleted: false,
        }
    }

    fn create_anonymous_client(&self) -> S3Client {
        if cfg!(feature = "disable_minio_unsupported") {
            // Minio does not support setting acls, so to make tests pass, return a client that has
            // the credentials of the bucket owner.
            self.s3.clone()
        } else {
            S3Client::new_with(
                rusoto_core::request::HttpClient::new().expect("Failed to creat HTTP client"),
                StaticProvider::from(AwsCredentials::default()),
                self.region.clone(),
            )
        }
    }

    fn create_test_bucket(&self, name: String) {
        let create_bucket_req = CreateBucketRequest {
            bucket: name.clone(),
            ..Default::default()
        };
        self.s3.create_bucket(create_bucket_req)
            .sync()
            .expect("Failed to create test bucket");
    }

    fn create_test_bucket_with_acl(&self, name: String, acl: Option<String>) {
        let create_bucket_req = CreateBucketRequest {
            bucket: name.clone(),
            acl,
            ..Default::default()
        };
        self.s3.create_bucket(create_bucket_req)
            .sync()
            .expect("Failed to create test bucket");
    }

    fn delete_object(&self, key: String) {
        let delete_object_req = DeleteObjectRequest {
            bucket: self.bucket_name.to_owned(),
            key: key.to_owned(),
            ..Default::default()
        };

        self.s3.delete_object(delete_object_req)
            .sync()
            .expect("Couldn't delete object");
    }

    fn put_test_object(&self, filename: String) {
        let contents: Vec<u8> = Vec::new();
        let put_request = PutObjectRequest {
            bucket: self.bucket_name.to_owned(),
            key: filename.to_owned(),
            body: Some(contents.into()),
            ..Default::default()
        };

        self.s3.put_object(put_request)
            .sync()
            .expect("Failed to put test object");
    }
}

impl Drop for StoreS3 {
    fn drop(&mut self) {
        if self.bucket_deleted {
            return;
        }
        let delete_bucket_req = DeleteBucketRequest {
            bucket: self.bucket_name.clone(),
            ..Default::default()
        };

        match self.s3.delete_bucket(delete_bucket_req).sync() {
            Ok(_) => println!("Deleted S3 bucket: {}", self.bucket_name),
            Err(e) => println!("Failed to delete S3 bucket: {}", e),
        }
    }
}
