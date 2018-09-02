currDir="$(pwd)"

if ! dpkg -s libbsd-dev > /dev/null || ! dpkg -s git > /dev/null
then
	echo Please install libssd-dev before running this script
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
	echo excellent > /dev/null/
else
	echo "Directory not found. Creating directory"
	mkdir "$PassFolders"
	cd "$PassFolders"
	echo $(pwd)
	PassFolders=$(pwd)
	cd "$currDir"
fi
echo $PassFolders

export PassFolders
git clone https://github.com/wolfSSL/wolfssl.git
cd wolfssl
./configure --enable-pwdbased --enable-dtls
make
make install
cd ..
make
cp src_files/everything "$PassFolders"
echo "We're almost done installing. Please go to your bashrc file add the "\
	"following line"
echo "export PATH=$PATH:\"$PassFolders/everything\""
echo "You should then be ready to use the program."
