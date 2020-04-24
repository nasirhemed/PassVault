use csv;
use std::{env, error::Error, io::{stdin, stdout, Write}, ffi::OsString, fs::File, process, collections::HashMap};
mod user;
use user::{*};


fn main() {
    let records = match run() {
        Ok(rdr) => rdr,
        Err(msg) => {
            println!("{}",msg);
            process::exit(1);
        }
    };
    loop {
        print!("Enter the domain name you would like to look for: ");
        stdout().flush().expect("Flush Failed");

        let mut domain_name = String::new();
        stdin().read_line(&mut domain_name).expect("Failed to read line");
        let domain_name = domain_name.trim().to_string();
        if domain_name == "" {
            break;
        }
        find_domain(&records, domain_name);

    }

}

fn find_domain(records : &HashMap<String, User>, domain_name: String) {

    let record = records.get(&domain_name);
    if let Some(record) = record {
        println!("Domain : {}\nUsername : {}\nPassword: {}", domain_name, record.username, record.password);
    }
    else {
        println!("Domain Not found");
    }

}

fn run() -> Result<std::collections::HashMap<String, user::User>, Box<dyn Error>> {
    let file_path = get_first_arg()?;
    let file = File::open(file_path)?;
    let mut rdr : csv::Reader<std::fs::File>  = csv::Reader::from_reader(file);
    let mut records = HashMap::new();

    for result in rdr.deserialize() {
        let record : user::UserInfo = result?;
        records.insert(record.domain, user::User::new(&record.username, &record.password));

    }
    Ok(records)
}

// Returns the first positional argument sent to this process. If there are no
// positional arguments, then this returns an error.
fn get_first_arg() -> Result<OsString, Box<dyn Error>> {
    match env::args_os().nth(1) {
        None => Err(From::from("expected 1 argument, but got none")),
        Some(file_path) => Ok(file_path),
    }
}