table! {
    chats (id) {
        id -> Integer,
        title -> Text,
        username -> Nullable<Text>,
        invite_link -> Nullable<Text>,
    }
}

table! {
    message_text (id) {
        id -> Integer,
        external_id -> Integer,
        created_at -> Integer,
        creator -> Nullable<Integer>,
        chat -> Nullable<Integer>,
        data -> Nullable<Text>,
    }
}

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

allow_tables_to_appear_in_same_query!(
    chats,
    message_text,
    users,
);
