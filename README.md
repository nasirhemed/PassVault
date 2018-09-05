# PassVault
PassVault is a password manager that securely stores your passwords in an encrypted "vault". It is a program designed for the terminal to store your passwords securely in an encrypted file.
The file is a CSV file encrypted using AES-256 and can be only opened by a key (Which is going to be your master password.). Once you decrypt the file, you can have
access to your passwords by letting the program know which domain you want to access. You can also add and generate passwords for your other domains.

The goal of this program is to encourage you to remember only one strong password for decrypting
the file and generate passwords for your domains so that you can remain safe.

## Getting Started
These instructions will get you a copy of the project up and running on your local machine.

### Prerequisites

* Git
* libsdd-dev

### Installing 

1. Clone the repository in a directory by the following command: `git clone https://github.com/nasirhemed/PassVault.git`
2. Download [wolfSSL](https://www.wolfssl.com/download/) and place the directory `wolfssl` in the directory you cloned PassVault
3. Run `./install.sh`
4. Run vault -c and enter your username and password

## Dependencies

1. Wolfssd for AES-256 encryption/decryption algorithms
2. libsdd for a proper random generator in C.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
