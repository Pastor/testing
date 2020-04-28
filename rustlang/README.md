1. ~/.cargo/config:
  ```toml
    [https]
    check-revoke = false
    sslVerify = false
    
    [http]
    check-revoke = false
    sslVerify = false
    
  ```

2. `cargo install cargo-binutils`

3. `rustup component add llvm-tools-preview`

4. `rustup target add thumbv7m-none-eabi`



