use bytes::Buf as _;
use futures::executor::block_on;
use http_auth_basic::Credentials;
use hyper::header::{
    HeaderValue, ACCEPT, AUTHORIZATION, CONTENT_TYPE, COOKIE, ETAG, LAST_MODIFIED, USER_AGENT,
};
use hyper::{Body, Client, HeaderMap, Request, Response};
use hyper_tls::HttpsConnector;
use std::str::FromStr;
use std::time::Duration;

const APPLICATION_JSON: &'static str = "application/json;charset=utf-8";
const APPLICATION_HAL_JSON: &'static str = "application/hal+json;v=2.0";

type Result<T> = std::result::Result<T, Box<dyn std::error::Error + Send + Sync>>;

pub struct Api {
    url: String,
    username: String,
    password: String,
    cookie: Option<String>,
}

impl Default for Api {
    fn default() -> Self {
        Api::new(
            "https://github.com".to_string(),
            "Default User".to_string(),
            "robotics".to_string(),
        )
    }
}

impl Api {
    pub fn new(url: String, username: String, password: String) -> Self {
        Api {
            url,
            username,
            password,
            cookie: None,
        }
    }

    fn etag(head: &HeaderMap) -> Option<&HeaderValue> {
        head.get(ETAG)
    }

    /// Extract the Last-Modified from the Headers if it exists
    fn last_modified(head: &HeaderMap) -> Option<&HeaderValue> {
        head.get(LAST_MODIFIED)
    }

    /// Extract however many requests the authenticated user can
    /// do from the Headers
    fn rate_limit_remaining(head: &HeaderMap) -> Option<u32> {
        head.get("X-RateLimit-Remaining")
            .map(|limit| u32::from_str(limit.to_str().unwrap_or("")).ok())
            .unwrap_or(None)
    }

    /// Extract however many requests the authenticated user can
    /// make from the Headers
    fn rate_limit(head: &HeaderMap) -> Option<u32> {
        head.get("X-RateLimit-Limit")
            .map(|limit| u32::from_str(limit.to_str().unwrap_or("")).ok())
            .unwrap_or(None)
    }

    /// Extract when the requests limit for the authenticated user
    /// is reset from the Headers
    fn rate_limit_reset(head: &HeaderMap) -> Option<u32> {
        head.get("X-RateLimit-Reset")
            .map(|limit| u32::from_str(limit.to_str().unwrap_or("")).ok())
            .unwrap_or(None)
    }

    fn cookie(head: &HeaderMap) -> Option<String> {
        head.get(COOKIE)
            .map(|cookie| Some(cookie.to_str().unwrap().to_string()))
            .unwrap_or(None)
    }

    fn request(&mut self, method: &'static str) -> Result<Response<Body>> {
        let mut request = Request::builder()
            .method(method)
            .uri(self.url.clone())
            .header(CONTENT_TYPE, HeaderValue::from_static(APPLICATION_JSON))
            .header(USER_AGENT, HeaderValue::from_static("abb-robo_v1"))
            .header(ACCEPT, HeaderValue::from_static(APPLICATION_HAL_JSON));

        match self.cookie {
            Some(ref value) => request = request.header(COOKIE, value),
            None => {
                let credentials = Credentials::new(
                    self.username.clone().as_str(),
                    self.password.clone().as_str(),
                );
                let credentials = credentials.as_http_header();
                request = request.header(AUTHORIZATION, credentials)
            }
        }
        let request = request.body(Body::empty())?;

        let https = HttpsConnector::new();

        let client = Client::builder()
            .pool_idle_timeout(Duration::from_secs(30))
            .http2_only(false)
            .build(https);

        let res = block_on(client.request(request))?;
        self.cookie = Api::cookie(res.headers());
        Ok(res)
    }

    pub fn users(&mut self) -> Result<Vec<UserInfo>> {
        let res = self.request("GET")?;
        let buf = block_on(hyper::body::aggregate(res)).unwrap();
        let users = serde_json::from_reader(buf.reader())?;
        tracing::info!("{:#?}", users);
        Ok(users)
    }
}

#[derive(Deserialize, Debug)]
pub struct UserInfo {
    id: i32,
    name: String,
}
