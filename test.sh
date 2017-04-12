sourceFile="example.txt"
destinationFile="destination.txt"
password="password"
username="dave"

#reset files
if ! [ -f $destinationFile ]; 
then
	touch $destinationFile 
fi
if ! [ -f $sourceFile ];
then
	touch $sourceFile
	echo "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum." >> $sourceFile
fi
> $destinationFile


#build
g++ -g3 -Wall -w  -Wextra cryptopp-authenc.cpp  -o cryptopp-authenc.exe -lcryptopp

#encrypt and decrypt
./cryptopp-authenc.exe encrypt $sourceFile $password $destinationFile $username
./cryptopp-authenc.exe decrypt $destinationFile $password $destinationFile $username

#test input / output
if cmp -s $sourceFile $destinationFile
then 
   echo "SUCCESS"
else
   echo "FAILURE: see files for details"
   exit
fi

#keep working directory clean
rm cryptopp-authenc.exe
rm $destinationFile
