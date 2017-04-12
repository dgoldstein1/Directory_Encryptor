all:
	g++ -g3 -Wall -w  -Wextra cryptopp-authenc.cpp  -o cryptopp-authenc.exe -lcryptopp

clean:
	rm cryptopp-authenc.exe