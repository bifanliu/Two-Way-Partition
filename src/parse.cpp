# include "parse.h"

DataBuffer* ReadFile(std::string Address) {
    // set temp DataBuffer
    DataBuffer *Data = new DataBuffer;

    // open file
    std::ifstream inputFile(Address);

    // check file open or not
    if (!inputFile.is_open()) {
        std::cerr << "Can't open file: " << Address << std::endl;
        return nullptr;
    }

    // read file content
    std::string line;
    if (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        if (!(iss >> Data->Num_Net >> Data->Num_Edge)) {
            std::cerr << "Error parsing the first line of the file." << std::endl;
            delete Data; // If an error occurs, delete the previously allocated DataBuffer
            return nullptr;
        }
    } else {
        std::cerr << "Error reading the first line of the file." << std::endl;
        delete Data; // If an error occurs, delete the previously allocated DataBuffer
        return nullptr;
    }

    // Initialize the size of NetList
    Data->NetList.resize(Data->Num_Net);

    // Read the remaining content of the file
    int netIndex = 0;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        int value;
        while (iss >> value) {
            Data->NetList[netIndex].push_back(value-1);
        }
        netIndex++;
    }

    // close file
    inputFile.close();

    // return value
    return Data;
}

void DataBuffer::PrintData(){
    std::cout << "Num_Net: " << Num_Net << std::endl;
    std::cout << "Num_Edge: " << Num_Edge << std::endl;

    // Output NetList
    for (int i = 0; i < Num_Net; ++i) {
        std::cout << "NetList[" << i << "]: ";
        for (int j : NetList[i]) 
            std::cout << j << " ";
        std::cout << std::endl;
    }
}

void printhypergraph(std::vector<std::vector<int>> & Hypergraph){
    for(int i = 0; i < (int)Hypergraph.size(); i++){
        printf("Edge %d: ", i+1);
        for(int j = 0; j < (int)Hypergraph[i].size(); j++)
            printf("%d ", Hypergraph[i][j]+1);
        printf("\n");
    }
}

void OutputFinalResult(std::string OutputName, struct WayBuffer *WayData){
    // open file
    std::ofstream outputFile(OutputName);
    for(int i = 0; i < (int)WayData->Bottom.size(); i++){
        if(WayData->Bottom[i] == true)
            outputFile << "1" << std::endl;
        else
            outputFile << "0" << std::endl;
    }
}