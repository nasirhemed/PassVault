currDir="$(pwd)"

if ! dpkg -s libbsd-dev > /dev/null || ! dpkg -s git > /dev/null
then
	echo Please install libbsd-dev before running this script
	exit 1
fi
if ! test -d wolfssl
then
	echo Please download wolfssl, rename the directory to wolfssl and place\ it here
	exit 1
fi

echo "Enter the path directory to store your encrypted passwords"
echo "Note: If nothing is specified, a default path (~/passFiles) will be "\
	"created"
read PassFolders
if test -z $PassFolders
then
	if ! mkdir ~/passFiles/ 2> /dev/null; then
		echo "Folder already exists. Would you like to continue? (Y/N)"
		read y
		if test $y != Y && test $y != y
		then
			echo "Cancelling installation"
			exit 1
		fi

	fi
	PassFolders=~/passFiles/	
fi
echo $PassFolders
if test -e $PassFolders
then
	echo excellent > /dev/null
else
	echo "Directory not found. Creating directory"
	mkdir "$PassFolders"
	cd "$PassFolders"
	echo $(pwd)
	PassFolders=$(pwd)
	cd "$currDir"
fi
echo $PassFolders

echo "export PassFolders=$PassFolders" >> ~/.bashrc
cd wolfssl
./configure --enable-pwdbased --enable-dtls
sudo make
sudo make install
cd /usr/local/lib/
sudo cp libwolfssl.la /usr/lib/
sudo cp libwolfssl.so /usr/lib
sudo cp libwolfssl.so.18 /usr/lib
sudo cp libwolfssl.so.18.0.0 /usr/lib
cd "$currDir"
make
cp src_files/vault "$PassFolders"
echo "#Add vault to the path" >> ~/.bashrc
echo 'export PATH=$PATH:$PassFolders' >> ~/.bashrc
make clean
echo "Enter the command 'source ~/.bashrc'."
echo "You should then be ready to use the program."
