table! {
    users (id) {
        id -> Integer,
        first_name -> Text,
        last_name -> Nullable<Text>,
        username -> Nullable<Text>,
        is_bot -> Nullable<Bool>,
        language_code -> Nullable<Text>,
    }
}
