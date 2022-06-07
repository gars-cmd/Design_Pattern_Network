## Matala6
---
- In this assignment we will implement  different design pattern  

### How to run:
---
1. First unzip the folder.
2. Open a terminal into the folder of the assignment 
3. run `make all` to create the executable files

#### Active-Object:
---
1. In the terminal run the instance of the server with `./server`
2. Open a new terminal in the same folder and run `./client localhost`
3. Then write a word upper/down case only and get the result (connection is non-persistent)
4. To launch a test run `./clientTest localhost`

#### Guard:
---
1. In the terminal run `./guard`
2. The answer to the question is at the bottom of the file guard.cpp

#### Singleton:
---
1. In the terminal run `./singleton`
2. The result is comparison of the memory of the instance created 

#### Reactor:
---
1. In the  terminal run `./selectserver`
2. In another terminal in the same folder run `./selectclient localhost`
> Notice : the selectserver do not support disconnection from client (result in select :  error bad file descriptor)




