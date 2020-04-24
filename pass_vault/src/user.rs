// use serde::Deserialize;
// This lets us write `#[derive(Deserialize)]`.
use serde::{Deserialize, Serialize};


#[derive(Debug, Deserialize, Serialize)]
#[serde(rename_all = "PascalCase")]
pub struct UserInfo {
  pub domain : String,
  pub username: String,
  pub password: String,
}

#[derive(Debug, Deserialize, Serialize)]
#[serde(rename_all = "PascalCase")]
pub struct User {
  pub username : String,
  pub password : String
}

impl User {
  pub fn new(username: &str, password: &str) -> User {
    User {
      username : username.to_string(),
      password : password.to_string()
    }
  }
}