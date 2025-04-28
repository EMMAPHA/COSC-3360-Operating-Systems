//Server
//Code is provided by Dr. Rincon from Canvas
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>

using namespace std;

// Handle zombie processes created by child processes
void fireman(int){
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// Struct to store the data for each row that needs to be decoded
struct ImageRowThreadData{
    int length, row;  // Image dimensions and current row index
    vector<int> headPos, dataPos;  // head positions and data positions
    vector<string>* ASCIIDrawing2Dimage; // Reference to the final image container
    vector<pair<char, vector<pair<int, int>>>> symbols; // Symbols and their ranges
};

// Function to decode a specific row of the ASCII image
string decodeRow(int row, int length, vector<int> headPos, vector<int> dataPos, 
                vector<pair<char, vector<pair<int, int>>>> symbols) {
    
    string decodedRow(length, ' '); // Initialize row with spaces (empty row)
    
    // Determine the start and end index for this row based on headPos
    int beginIndex = 0;
    int endIndex = dataPos.size();
    
    // Find the correct begin index for this row
    if (row < headPos.size()) {
        beginIndex = headPos[row];
        
        // Find the end index (start of next row or end of dataPos)
        if (row + 1 < headPos.size()) {
            endIndex = headPos[row + 1];
        }
    } else {
        // If row is out of bounds, return an empty row
        return decodedRow;
    }
    
    // Place symbols in the row based on dataPos values and symbol ranges
    for (const auto& symbolData : symbols) {
        char symbol = symbolData.first; // Get the symbol
        
        // Check if data positions fall within the symbol's defined ranges
        for (int j = beginIndex; j < endIndex; j++) {
            int x = dataPos[j]; // Get the x position
            // Check if this position falls within any of the ranges for this symbol
            for (const auto& range : symbolData.second) {
                if (x >= range.first && x <= range.second && x < length) {
                    decodedRow[x] = symbol;  // Place the symbol at position x
                }
            }
        }
    }
    
    return decodedRow;
}

// Parse the input from client and decode the row (creates buffer string used to send back to client)
string solve(string buffer) {
    stringstream ss(buffer);
    int row, length;
    
    // Read row index and length
    ss >> row >> length;
    
    // Read number of symbols and and sizes of headPos and dataPos
    int numSymbols, headPosSize, dataPosSize;
    ss >> numSymbols >> headPosSize >> dataPosSize;
    
    // Read symbols and their ranges from the buffer
    vector<pair<char, vector<pair<int, int>>>> symbols;
    for (int i = 0; i < numSymbols; i++) {
        char symbol;
        int numRanges;
        ss >> symbol >> numRanges;
        
        vector<pair<int, int>> ranges;
        for (int j = 0; j < numRanges; j++) {
            int start, end;
            ss >> start >> end;
            ranges.emplace_back(start, end);
        }
        symbols.emplace_back(symbol, ranges);
    }
    
    // Read headPos values from the buffer
    vector<int> headPos(headPosSize);
    for (int i = 0; i < headPosSize; i++) {
        ss >> headPos[i];
    }
    
    // Read dataPos values from the buffer
    vector<int> dataPos(dataPosSize);
    for (int i = 0; i < dataPosSize; i++) {
        ss >> dataPos[i];
    }
    
    // Call the row decoding function and return the result
    return decodeRow(row, length, headPos, dataPos, symbols);
}


int main(int argc, char *argv[]){
   int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   
   // Check the commandline arguments
   if (argc != 2)
   {
      std::cerr << "Port not provided" << std::endl;
      exit(0);
   }

   // Create the socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0)
   {
      std::cerr << "Error opening socket" << std::endl;
      exit(0);
   }

   // Populate the sockaddr_in structure
   bzero((char *)&serv_addr, sizeof(serv_addr));
   portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   // Bind the socket with the sockaddr_in structure
   if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
   {
      std::cerr << "Error binding" << std::endl;
      exit(0);
   }

   // Set the max number of concurrent connections
   listen(sockfd, 5);
   clilen = sizeof(cli_addr);

    signal(SIGCHLD, fireman);   
   // Accept a new connection
   while(true) {
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if(fork() == 0 ) {
      if (newsockfd < 0)
      {
          std::cerr << "Error accepting new connections" << std::endl;
          exit(0);
      }
      int n, msgSize = 0;
      
      n = read(newsockfd, &msgSize, sizeof(int));
      if (n < 0)
      {
          std::cerr << "Error reading from socket" << std::endl;
          exit(0);
      }
      char *tempBuffer = new char[msgSize + 1];
      bzero(tempBuffer, msgSize + 1);
      n = read(newsockfd, tempBuffer, msgSize + 1);
      if (n < 0)
      {
          std::cerr << "Error reading from socket" << std::endl;
          exit(0);
      }

      std::string buffer = tempBuffer;
      delete[] tempBuffer;
      buffer = solve(buffer); // Process the received data and decode the row
      msgSize = buffer.size();
      n = write(newsockfd, &msgSize, sizeof(int));
      if (n < 0)
      {
          std::cerr << "Error writing to socket" << std::endl;
          exit(0);
      }
      n = write(newsockfd, buffer.c_str(), msgSize);
      if (n < 0)
      {
          std::cerr << "Error writing to socket" << std::endl;
          exit(0);
      }
      // Child process ends here
      close(newsockfd);
      _exit(0);
    }
    wait(nullptr);
   }
   
   //Close sockets
   //close(newsockfd);
   close(sockfd);
   return 0;
}