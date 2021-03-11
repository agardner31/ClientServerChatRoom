Anna Gardner ag@gatech.edu

Files Submitted:
- chatclient.c
- chatserver.c
- README.txt
- makefile

Hello TA. I hope you are having a wonderful day.
This chat room was written in C to run on the newest version of Ubuntu 20.04.2.

Instructions to run my code:

Navigate to the folder with chat files in your directory and run 'make' in your terminal/command prompt to compile the code
To start the server, run './chatserver' on your terminal/command prompt.
To join the chat room as a new user and connect to the server, open a new terminal and run './chatclient -username <username> -password <password>
Type any messages you want to send as a user and press enter to send. Special 'emoji' messages will be sent as the text translation.
Messages sent will be logged by the server.
Type 'Exit' to end connection and leave chat room.

To get rid of compiled files for any reason, use 'make clean' command.
To shut down server use Ctrl C.

Unfortunately, after many many hours of debugging, I couldn't get the multithreading to work.
Thus, both functionalities of multiple clients connecting to the server and the server broadcasting 
any messages sent do not work. You can look in my server and chat files to see that I had a broadcasting method,
data structure to keep track of all the clients connected to the server, and anything else I could need to make it work... I just couldn't figure out what was wrong.
While I understand that I cannot get credit for the functionalities that I do not have, I just wanted to point out
that I gave it the full effort :)