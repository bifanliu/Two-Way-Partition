// main.cpp
#include <chrono>

#include "V_cycle.h"

#define CASE1 "benchmarks/case1.txt"
#define CASE2 "benchmarks/case2.txt"
#define CASE3 "benchmarks/case3.txt"
#define OUTPUTNAME "output.txt"

extern int Layer_ctr;

int main() {
    // get start time
    auto start_time = std::chrono::high_resolution_clock::now();
    // get file net and cell
    DataBuffer* data = ReadFile(CurrentCase);

    /* Check if the file was successfully read
       if (data != nullptr) {
           data->PrintData();
       }
    */

   // create First Layer data because it doesn't change
   struct FCBuffer *FirstLayer = new struct FCBuffer;
    FirstLayer->preFC = NULL;
    FirstLayer->NetList = data->NetList;
    FirstLayer->Vertecies = std::vector<std::vector<int>> (data->Num_Edge);
    FirstLayer->VerteciesSize = std::vector<int> (data->Num_Edge, 1);

    // find HyperGraph
    std::vector<std::vector<int>> HyperGraph(FirstLayer->Vertecies.size());
    for(int i = 0; i < (int)FirstLayer->NetList.size(); i++){
        for(int j = 0; j < (int)FirstLayer->NetList[i].size(); j++)
            HyperGraph[FirstLayer->NetList[i][j]].push_back(i);
    }
    FirstLayer->HyperGraph = HyperGraph;
   std::ofstream SaveFinalCut("Final_Cut.txt");

    int count = 0;
    int totalrun = 0;
    while(totalrun < RUNS){
        count = 0;
        WayBuffer *BestWayData = NULL;
        while(count < Nruns){

            // Init First layer verticies
            for(int i = 0; i < data->Num_Edge; i++)
                FirstLayer->Vertecies[i].push_back(i);
            
            // Create Current FC
            struct FCBuffer *CurrentFC = FirstLayer;
            // record layer count
            int layer_ctr = 0;

            // if Edge > C*K than keep Coarsening
            if(data->Num_Edge > C*K){
                // coarsening phase
                while ((int)CurrentFC->Vertecies.size() > C*K){
                    // create next layer FC
                    CurrentFC->nxtFC = new struct FCBuffer;
                    // Remember to mark the current layer as the previous layer of the next layer
                    CurrentFC->nxtFC->preFC = CurrentFC;
                    CurrentFC = RunFCCoarsening(CurrentFC, layer_ctr);
                    layer_ctr++;
                    Layer_ctr = layer_ctr;
                }
                
                // make last currentFC HyperGraph
                CurrentFC->HyperGraph = std::vector<std::vector<int>> (CurrentFC->Vertecies.size());
                for(int i = 0; i < (int)CurrentFC->NetList.size(); i++){
                    for(int j = 0; j < (int)CurrentFC->NetList[i].size(); j++)
                        CurrentFC->HyperGraph[CurrentFC->NetList[i][j]].push_back(i);
                }
            }

            // make sure all size 
            int TotalSize = 0;
            for(int i = 0; i < (int)CurrentFC->Vertecies.size(); i++){
                TotalSize += CurrentFC->VerteciesSize[i];
                // printf("Vertecies[%d] = %d\n", i, CurrentFC->VerteciesSize[i]);
            }
            if(TotalSize != data->Num_Edge)
                printf("Real Vertecies Size is %d, not %d\n", data->Num_Edge, TotalSize);
            
            // get end time
            auto end_time = std::chrono::high_resolution_clock::now();

            // evaluate total cost time
            std::chrono::duration<double> elapsed_time = end_time - start_time;
            std::cout << "Total Run Time is : " << elapsed_time.count() << " second " << std::endl;

            // Init Way Partiton 
            struct FCBuffer *TempCurrentFC = CurrentFC;
            int TempLayerCtr = layer_ctr;
            for(int i = 0;i < 5; i++){
                CurrentFC = TempCurrentFC;
                layer_ctr = TempLayerCtr;
                struct WayBuffer *WayData;
                WayData = InitPartition(CurrentFC, TotalSize, 0.01*i);
                // PrintFinalWayResult(WayData);

                // start use FM algorithm to uncoarsening
                while(layer_ctr >= 0){
                    *WayData = FM(CurrentFC, WayData, layer_ctr);
                    layer_ctr--;
                    // *WayData = K_LIN(CurrentFC, WayData);
                    // first layer didn't updata WayData
                    if(layer_ctr < 0)
                        break;
                    else
                        // Update Way Data to previous vertecies format
                        ChangeWayData(CurrentFC, WayData);
                    // change CurrentFC to previous FC
                    CurrentFC = CurrentFC->preFC;
                }

                // Use K-Lin Optimal Final solution
                // *WayData = K_LIN(CurrentFC, WayData);

                WayData->Cut = EvalCut(CurrentFC, WayData);
                printf("\n********************\n");
                printf("* Final Cut is %3d *\n", WayData->Cut);
                printf("********************\n");

                if(BestWayData == NULL)
                    BestWayData = WayData;
                else if(WayData->Cut < BestWayData->Cut)
                    BestWayData = WayData;
                else
                    delete WayData;
            }
            count++;
        }

        if((float)BestWayData->BottomSize / (float)(BestWayData->BottomSize + BestWayData->TopSize) > Tratio ||
            (float)BestWayData->BottomSize / (float)(BestWayData->BottomSize + BestWayData->TopSize) < Bratio)
        SaveFinalCut << "Error" << std::endl;
        else
            SaveFinalCut <<  BestWayData->Cut << std::endl; 
    
        // Output Final Result
        printf("\n********************\n");
        printf("* Final Best Cut is %3d *\n", BestWayData->Cut);
        printf("********************\n");
        totalrun++;
        OutputFinalResult(OUTPUTNAME, BestWayData);
    }

    // call verifier
    // char command[100];
    // snprintf(command, sizeof(command), "./verifier %s output.txt", CurrentCase);
    // system(command);

    // Release dynamically allocated DataBuffer
    delete data;

    // get end time
    auto end_time = std::chrono::high_resolution_clock::now();

    // evaluate total cost time
    std::chrono::duration<double> elapsed_time = end_time - start_time;
    std::cout << "Total Run Time is : " << elapsed_time.count() << " second " << std::endl;


    return 0;
}
