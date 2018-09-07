# PassVault
PassVault is a password manager that securely stores your passwords in an encrypted "vault". It is a program designed for the terminal to store your passwords securely in an encrypted file.
The file is a CSV file encrypted using AES-256 and can be only opened by a key (Which is going to be your master password.). Once you decrypt the file, you can have
access to your passwords by letting the program know which domain you want to access. You can also add and generate passwords for your other domains.

![2018-09-06_22-09-27](https://user-images.githubusercontent.com/33851317/45194841-d0696380-b222-11e8-9af8-2e70044aa667.gif)


The goal of this program is to encourage you to remember only one strong password for decrypting
the file and generate passwords for your domains so that you can remain safe.

## Usage
Once you create an account using `vault -c`, you can do the following:

* If you want to add your own passwords or generated passwords then type the command `vault -w`.
* If you want to see the passwords you stored, you type the command `vault -r` and type the domain that you are looking for.
* If you want to generate your password file into a readable CSV file, then type `vault -g` (**Note: If you generate the CSV file, then all your passwords will be visible.**)
* If you want to create another account, then simply type the command `vault -c` again and use a different username


## Getting Started
These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

* Git
* libbsd-dev

### Installing 

1. Clone the repository in a directory by the following command: `git clone https://github.com/nasirhemed/PassVault.git`
2. Download [wolfSSL](https://www.wolfssl.com/download/) and place the directory `wolfssl` in the directory you cloned PassVault
3. Run `./install.sh`
4. Run vault -c and enter your username and password

## Dependencies

* [wolfSSL](https://www.wolfssl.com) for AES-256 encryption/decryption algorithms
* [libBSD](https://github.com/LuaDist/libbsd) for a proper random generator in C.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
