//Client
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

// Struct to store the data for each row that needs to be decoded
struct ImageRowThreadData{
    int length, row;  // Image dimensions and current row index
    vector<int> headPos, dataPos;  // head positions and data positions
    vector<string>* ASCIIDrawing2Dimage; // Reference to the final image container
    vector<pair<char, vector<pair<int, int>>>> symbols; // Symbols and their ranges
    char* serverIP;    // Server IP from argv[1]
    char* port;      // Server port from argv[2]
};

// Thread function to send data to the server and receive the decoded row
void* decodingRow(void* arg){
    
    //initializing variables and casting
    struct ImageRowThreadData* data = (struct ImageRowThreadData*)arg;
    
    // Set up socket connection
    int sockfd, portno, n;
    std::string buffer;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    

    //portno = atoi(argv[2]);
    portno = atoi(data->port); // Convert port to integer
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //creating socket
    if (sockfd < 0) 
    {
        std::cerr << "ERROR opening socket" << std::endl;
        exit(0);
    }
    // server = gethostbyname(argv[1]);
    server = gethostbyname(data->serverIP);
    if (server == NULL) {
        std::cerr << "ERROR, no such host" << std::endl;
        exit(0);
    }
    
    // Initialize the socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    
     // Connect to server
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
    {
        std::cerr << "ERROR connecting" << std::endl;
        exit(0);
    }
    
     // Prepare the data to send to the server (row, length, symbols, headPos, dataPos)
    stringstream ss;
    ss << data->row << " " << data->length << " ";
    ss << data->symbols.size() << " " << data->headPos.size() << " " << data->dataPos.size() << " ";
    
    // Include the symbols and their ranges
    for (const auto& symbolData : data->symbols) {
        ss << symbolData.first << " " << symbolData.second.size() << " ";
        for (const auto& range : symbolData.second) {
            ss << range.first << " " << range.second << " ";
        }
    }
    
    // Include headPos values
    for (int i = 0; i < data->headPos.size(); i++) {
        ss << data->headPos[i] << " ";
    }
    
    // Include dataPos values
    for (int i = 0; i < data->dataPos.size(); i++) {
        ss << data->dataPos[i] << " ";
    }
    
    buffer = ss.str();
    
    // Send the buffer size and buffer content to the server
    // writing to server
    int msgSize = buffer.size();
    n = write(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    n = write(sockfd,buffer.c_str(),msgSize);
    if (n < 0) 
    {
        std::cerr << "ERROR writing to socket" << std::endl;
        exit(0);
    }
    // Receive the decoded row from the server
    //reading from server
    n = read(sockfd,&msgSize,sizeof(int));
    if (n < 0) 
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    char *tempBuffer = new char[msgSize+1];
    bzero(tempBuffer,msgSize+1);
    n = read(sockfd,tempBuffer,msgSize);
    if (n < 0) 
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(0);
    }
    buffer = tempBuffer;
    
    //Store the server's returned row (Store the decoded row into the final image array)
    (*data->ASCIIDrawing2Dimage)[data->row] = string(tempBuffer);
    
    
    delete [] tempBuffer;
    close(sockfd);
    
    return nullptr;
    
}

int main(int argc, char *argv[]) {
    
    // Check if there are enough command-line arguments
     if (argc != 3) {
       std::cerr << "usage " << argv[0] << " hostname port" << std::endl;
       exit(0);
     }
    
    int length, width;  // Image dimensions: length (length of row) and width (number of rows)
    string imageLine;

    // Reads in the image dimensions
    getline(cin, imageLine);
    stringstream ss(imageLine);
    ss >> length >> width;

     // Reads in the symbol definitions (character and ranges)
    vector<pair<char, vector<pair<int, int>>>> symbols;
    getline(cin, imageLine);
    stringstream ssSecond(imageLine);
    string token;

    while(getline(ssSecond, token, ',')){ // Split the symbol data by commas
        stringstream ssThird(token);
        char symbol;
        ssThird >> symbol; // Extract the symbol
        vector<pair<int, int>> ranges;
        int begin, end;
        while(ssThird >> begin >> end) // Extract the ranges for this symbol
            ranges.emplace_back(begin, end);  // Add the ranges for the symbol
        symbols.emplace_back(symbol, ranges); // Stores the symbol and its ranges
    }

    // Reads in the headPos (Reads in head positions of the image)
    vector<int> headPos, dataPos;
    getline(cin, imageLine);
    stringstream ssThird(imageLine);
    int numbers;
    while(ssThird >> numbers){
        headPos.push_back(numbers); // Stores the head positions (y-axis)
    }

     // Reads in the dataPos (Reads in data positions of the image)
    getline(cin, imageLine);
    stringstream ssForth(imageLine);
    while(ssForth >> numbers){
        dataPos.push_back(numbers); // Store data positions (x-axis)
    }

    vector<string> ASCIIDrawing2Dimage(width); // Initializes the image storage

    // Create and initialize the threads to process each row
    pthread_t theThreads[width];
    vector<ImageRowThreadData> myThread(width); // Vector to hold data for each row's thread

    for(int j = 0; j < width; j++){
        // Set up the data for the current row's thread
        myThread[j].row = j;
        myThread[j].length = length;
        myThread[j].headPos = headPos;
        myThread[j].dataPos = dataPos;
        myThread[j].ASCIIDrawing2Dimage = &ASCIIDrawing2Dimage;
        myThread[j].symbols = symbols;
        
        myThread[j].serverIP = argv[1];
        myThread[j].port = argv[2];

        if(pthread_create(&theThreads[j], nullptr, decodingRow, &myThread[j])){ // Create thread for each row
            fprintf(stderr, "Error creating thread\n");
            return 1; // Exit if thread creation fails
        }
    }


    // Wait for all threads to finish and join the threads
    for(int j = 0; j < width; j++){
        pthread_join(theThreads[j], nullptr);  // Wait for each thread to finish
    }

    // Print the decoded 2D image with printable ASCII characters
    for(const auto& row : ASCIIDrawing2Dimage){
         cout << row << endl; // Outputs each row of the image
     }
     
    return 0;
}