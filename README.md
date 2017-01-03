# chrome-passwords

Extract (Recover) locally saved passwords and cookies (with -p ) on Google Chrome and decrypt them.
C/C++ Implementation.

## How it works ?
Shortly, Google chrome stores your saved passwords in a SQLLite3 DB file in your local app chrome directory relevant to your user. each password is encrypted by whatever user specific credentials your OS provides.
on Windows, which this implementation is dedicated to, it uses Windows Crypt API functions, namely CryptProtectData which depends on a seed local to your user 
(in Microsoft words : logon credentials that match those of the user who encrypted the data can decrypt the data.), in order to ecrypt your password to a binary format stored in a blob column beside url , username and other information.
Sadly this credentials are identified only by your user, so any process running on your user prvillages level can encrypt/decrypt data.
When you ask chrome to show your saved password in plain text, it asks for your OS user's pass. This gives a false sense of security as it really does not matter ! 
just run my implementation and you will get list of everything you stored without a need for privillages elevation !.

Note: this comes after Google was using Plain text to store our passwords :-)
for more details: a good answer on Stackoverflow http://security.stackexchange.com/a/40887

Same applies for cookies
## How to use ?
* without any argument to display saved passwords as following : url \n username \n password 
* -p to disable waiting for random input at the end
(useful if you want to forward to the output to a file like chrome-passwords.exe -p > output)
* -c to show also cookies as following: host_key \n path \n cookie value

## Implementation
Application is compiled using Visual Studio C++ 2015 toolkit; target SDK 8.1 & OS: Win 7

Dependecies:
* SQLLite3 : The amalgamation version : sqlite3.c and sqlite3.h 
* Microsoft Cryptography API. Cryptlib32.lib ( You can get by installing Windows SDK if visual studio does not already have it).
 I prefer static linking for dependecies and CRT in such tiny app.

## Contributing
This is a fairly simple show case for a possibility of exploiting the ecnrypting mechanisim. 
If it does not work on your machine, feel free to send me a report of the problem.
Of course, this code meant to expose the issue and not intended for any harmful reasons.