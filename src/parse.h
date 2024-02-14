#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

class DataBuffer{
    public:
        // basic data
        int Num_Edge;
        int Num_Net;
        std::vector<std::vector<int>> NetList;
        
        // function
        void PrintData();
        
};

struct WayBuffer
{
    std::vector<bool> Top;
    std::vector<bool> Bottom;
    int TopSize = 0;
    int BottomSize = 0;
    int Cut;
};

DataBuffer* ReadFile(std::string Address);
void printhypergraph(std::vector<std::vector<int>> & Hypergraph);
void OutputFinalResult(std::string OutputName, struct WayBuffer *WayData);