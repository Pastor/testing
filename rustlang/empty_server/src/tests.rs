use rocket;
use rocket::http::{ContentType, Status};
use rocket::local::Client;

use crate::main_rocket;

const GUID: &str = "40969124-DAD8-4750-BE4A-7F8F11968B7B";
const SIG: &str = "WWREQkI1OHpVcjVWMXFUbDY0V29qSlBCdTQ2dDRRcDRGNDV0UXZKdUVOT3VFbjBNSzlnTXJ6cUpacm02NzE5TWVteHpzZ3FiWDZnSUQzMTNGVUlsSWVwY044Mlc3enkwdWlYdjdOamRpbDFHYzY2cmdvbGF0Sg";

#[test]
fn success_get() {
    let client = Client::new(main_rocket()).unwrap();

    let mut res = client.get(format!("/request?GUID={}&SIG={}", GUID, SIG)).dispatch();
    assert_eq!(res.status(), Status::Ok);

    let body = res.body_string().unwrap();
    let message_ret = format!("Proxy GUID: '{}', SIG: '{}'", GUID, SIG);
    assert!(body.contains(message_ret.as_str()));
}
