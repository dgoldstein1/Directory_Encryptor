#creates terminal command

mkdir ~/bin
cp . ~/bin/dir_encryptor -r
chmod 775 ~/bin/dir_encryptor/run.ssh 
echo "export PATH=$PATH:~/bin/dir_encryptor" >> ~/.bashrc