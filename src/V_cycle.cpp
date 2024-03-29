#include "V_cycle.h"

// record total layer if reset then counter will reset
int Layer_ctr;

struct FCBuffer *RunFCCoarsening(struct FCBuffer *CurrentFC, int layer_ctr){
    printf("\n====================\n");
    printf("[Coarsening %2d Times] ===================================================\n", layer_ctr);

    // state next FC pointer
    struct FCBuffer *nxtFC = CurrentFC->nxtFC;
    // The value of counter will affect the vertex concentration phenomenon
    static float counter = RATIO;
    // if loop many time counter will update
    if(layer_ctr == 0)
        counter = RATIO;
    
    // that time will flow
    srand(time(0));

    std::vector<std::vector<int>> HyperGraph(CurrentFC->Vertecies.size());
    if(layer_ctr != 0){
        // find HyperGraph
        for(int i = 0; i < (int)CurrentFC->NetList.size(); i++){
            for(int j = 0; j < (int)CurrentFC->NetList[i].size(); j++)
                HyperGraph[CurrentFC->NetList[i][j]].push_back(i);
        }
        CurrentFC->HyperGraph = HyperGraph;
    }
    else{
        HyperGraph = CurrentFC->HyperGraph;
    }
    
    // Is Vertecies Matching?(init all false)
    std::vector<bool> MatchVertecies (CurrentFC->Vertecies.size(), false);

    // Record Vertecies Into which NewVertecies set
    std::vector<int> RecordVerteciesNewPlace(CurrentFC->Vertecies.size(), -1);
    // Number of NewVertecies 
    int NumNewVertecies = 0;
    // Number of UnmatchVertecies
    int NumUnmatchVertecies = (int)MatchVertecies.size();
    // Record Unmatch Vertecies
    std::vector<int> UnmatchVertecies;
    // Init UnmatchVertecies

    for(int i = 0; i < NumUnmatchVertecies; i++)
        UnmatchVertecies.push_back(i);
    printf("start coarsening ...----------------------------------------------------|\n");

    while(NumUnmatchVertecies != 0){
        // find random vertecies
        int Index = rand() % NumUnmatchVertecies;

        // if random vertecies already match then swap UnmatchVertecies[Index] to last one and NumUnmatchVertecies minus one 
        while(MatchVertecies[UnmatchVertecies[Index]] != false){
            NumUnmatchVertecies--;
            if(NumUnmatchVertecies <= 0)
                break;
            // swap with last Vertecies
            int temp = UnmatchVertecies[NumUnmatchVertecies];
            UnmatchVertecies[NumUnmatchVertecies] = UnmatchVertecies[Index];
            UnmatchVertecies[Index] = temp;
            Index = rand() % NumUnmatchVertecies;
        }

        // if NumUnmatchVertecies is zero than over while loop because all vertecies match
        if(NumUnmatchVertecies <= 0)
            break;
        else
            Index = UnmatchVertecies[Index];
        
        /* In the net to which Index belongs, if net.size() is less than or equal to counter, 
         * find out which vertecies in these nets are most related to Index, that is, 
         * if the calculated Weight value is the maximum
        */
        std::vector<float> Weight(CurrentFC->Vertecies.size(), 0);
        float MaxWeightValue = INT16_MIN;
        int MaxWeightVertecies = -1;
        int MaxNetListCount;

        // match and unmatch both consider
        for(int i = 0; i < (int)HyperGraph[Index].size(); i++){
            if((int)CurrentFC->NetList[HyperGraph[Index][i]].size() <= counter && (int)CurrentFC->NetList[HyperGraph[Index][i]].size() > 1){
                for(int j = 0; j < (int)CurrentFC->NetList[HyperGraph[Index][i]].size(); j++){
                    if(CurrentFC->NetList[HyperGraph[Index][i]][j] != Index){
                        // record current vertex weight 
                        float CurrentWeight;
                        Weight[CurrentFC->NetList[HyperGraph[Index][i]][j]]+=(float)1/(float)(CurrentFC->NetList[HyperGraph[Index][i]].size());
                        CurrentWeight = Weight[CurrentFC->NetList[HyperGraph[Index][i]][j]];

                        // compare if currentweight bigger than maxweightvalue and change maxweightvertecies
                        // if equal with maxweightvalue and compare is match or not or Hypergraph size etc
                        if(CurrentFC->NetList[HyperGraph[Index][i]][j] != Index && CurrentWeight >= MaxWeightValue){
                            if(CurrentWeight > MaxWeightValue){
                                MaxWeightValue = CurrentWeight;
                                MaxWeightVertecies = CurrentFC->NetList[HyperGraph[Index][i]][j];
                            }
                            else if(MatchVertecies[CurrentFC->NetList[HyperGraph[Index][i]][j]] == false){
                                if(MatchVertecies[MaxWeightVertecies] == true && HyperGraph[CurrentFC->NetList[HyperGraph[Index][i]][j]].size() < HyperGraph[MaxWeightVertecies].size())
                                    MaxWeightVertecies = CurrentFC->NetList[HyperGraph[Index][i]][j];
                            }
                            else if(MatchVertecies[MaxWeightVertecies] == false && MatchVertecies[CurrentFC->NetList[HyperGraph[Index][i]][j]] == false && 
                                    HyperGraph[CurrentFC->NetList[HyperGraph[Index][i]][j]].size() < HyperGraph[MaxWeightVertecies].size())
                                MaxWeightVertecies = CurrentFC->NetList[HyperGraph[Index][i]][j];
                            else if(MatchVertecies[MaxWeightVertecies] == true && MatchVertecies[CurrentFC->NetList[HyperGraph[Index][i]][j]] == true && 
                                    HyperGraph[CurrentFC->NetList[HyperGraph[Index][i]][j]].size() < HyperGraph[MaxWeightVertecies].size())
                                MaxWeightVertecies = CurrentFC->NetList[HyperGraph[Index][i]][j];
                            else if(MatchVertecies[MaxWeightVertecies] == true && MatchVertecies[CurrentFC->NetList[HyperGraph[Index][i]][j]] == true && 
                                    CurrentFC->VerteciesSize[CurrentFC->NetList[HyperGraph[Index][i]][j]] < CurrentFC->VerteciesSize[MaxWeightVertecies])
                                MaxWeightVertecies = CurrentFC->NetList[HyperGraph[Index][i]][j];
                        }
                    }
                }
            }
        }

        // look have more relate vertecies connect with Index
        if(MaxWeightVertecies == -1 || RecordVerteciesNewPlace[MaxWeightVertecies] == -1){
            // create new vertecies
            std::vector<int> NewVertecies;
            // record new vertecies size
            int NewVerteciesSize = 0;
            // put index into newverteices
            RecordVerteciesNewPlace[Index] = NumNewVertecies;
            NewVertecies.push_back(Index);
            if(MaxWeightVertecies != -1 && RecordVerteciesNewPlace[MaxWeightVertecies] == -1){
                RecordVerteciesNewPlace[MaxWeightVertecies] = NumNewVertecies;
                NewVertecies.push_back(MaxWeightVertecies);
                MatchVertecies[MaxWeightVertecies] = true;
                NewVerteciesSize += CurrentFC->VerteciesSize[MaxWeightVertecies];
            }
            // update newvertecies size
            NewVerteciesSize += CurrentFC->VerteciesSize[Index];
            // change match to true
            MatchVertecies[Index] = true;
            // NewVertecies push into next FC
            nxtFC->Vertecies.push_back(NewVertecies);
            // push NewVertecies size
            nxtFC->VerteciesSize.push_back(NewVerteciesSize);
            // newvertecies size++
            NumNewVertecies++;
        }
        else{
            // put into new vertecies that this new vertecies is larger weight with Index
            nxtFC->Vertecies[RecordVerteciesNewPlace[MaxWeightVertecies]].push_back(Index);
            // change match to true
            MatchVertecies[Index] = true;
            // push NewVertecies size
            nxtFC->VerteciesSize[RecordVerteciesNewPlace[MaxWeightVertecies]] += CurrentFC->VerteciesSize[Index];
            // put index into newverteices
            RecordVerteciesNewPlace[Index] = RecordVerteciesNewPlace[MaxWeightVertecies];
        }
    }

    while(NumUnmatchVertecies >= 0){
        if(MatchVertecies[UnmatchVertecies[NumUnmatchVertecies]] == false){
            std::vector<int> NewVertecies;
            NewVertecies.push_back(UnmatchVertecies[NumUnmatchVertecies]);
            nxtFC->Vertecies.push_back(NewVertecies);
            nxtFC->VerteciesSize.push_back(CurrentFC->VerteciesSize[UnmatchVertecies[NumUnmatchVertecies]]);
            RecordVerteciesNewPlace[UnmatchVertecies[NumUnmatchVertecies]] = NumNewVertecies;
            NumNewVertecies++;
            MatchVertecies[UnmatchVertecies[NumUnmatchVertecies]] = true;
        }
        NumUnmatchVertecies--;
    }

    counter+=RATIOACCELERATOR;
    printf("counter is %5f                                                      |\n", counter);

    // make sure all origin unmatch vertecies is match
    for(int i = 0; i < (int)MatchVertecies.size(); i++){
        if(MatchVertecies[i] != true)
            printf("Vertecies %6d is not match                                           |\n", i+1);
    }

    printf("NewVertecies Size is %6d                                             |\n", (int)nxtFC->Vertecies.size());

    // udpate new netlist
    for(int i = 0; i < (int)CurrentFC->NetList.size(); i++){
        std::vector<bool> Vertecies(nxtFC->Vertecies.size(), false);
        std::vector<int> NewNet;
        int NewNetNum = 0;
        for(int j = 0; j < (int)CurrentFC->NetList[i].size(); j++){
            if(Vertecies[RecordVerteciesNewPlace[CurrentFC->NetList[i][j]]] == false){
                Vertecies[RecordVerteciesNewPlace[CurrentFC->NetList[i][j]]] = true;
                NewNetNum++;
                NewNet.push_back(RecordVerteciesNewPlace[CurrentFC->NetList[i][j]]);
            }
        }
        if(NewNetNum > 1){
            nxtFC->NetList.push_back(NewNet);
        }
    }
    printf("NewNetList Size is %5d                                                |\n", (int)nxtFC->NetList.size());


    printf("=========================================================================\n");
    return nxtFC;
}

struct WayBuffer *InitPartition(struct FCBuffer *CurrentFC, int TotalSize, float IterationRate){
    // create Temp way buffer
    struct WayBuffer *WayData = new struct WayBuffer;
    // Init WayData
    WayData->Top = std::vector<bool> (CurrentFC->Vertecies.size(), false);
    WayData->TopSize = 0;
    // Initially, let all vertecies be concentrated in a certain way
    WayData->Bottom = std::vector<bool> (CurrentFC->Vertecies.size(), true);
    WayData->BottomSize = TotalSize;

    // Create AdjMatrix
    int Cut = 0;
    int MinCutIncreaseIndex, TempCut;
    int MinCut = 0;
    int IterationTime = 0;
    int BottomLimit = (WayData->BottomSize + WayData->TopSize) * (Bratio+IterationRate);
    int BottomLimitest = (WayData->BottomSize + WayData->TopSize) * (Bratio+IterationRate);
    int CurrentSize = WayData->BottomSize + WayData->TopSize;
    while(CurrentSize > BottomLimit){
        float CutSizeRatio = INT16_MAX;
        MinCut = INT16_MAX;
        IterationTime++;
        for(int i = 0; i < (int)WayData->Bottom.size(); i++){
            // if Vertecies move to another way how much cut will increase
            if(WayData->Bottom[i] == true && (CurrentSize - CurrentFC->VerteciesSize[i]) > BottomLimitest){
                WayData->Bottom[i] = false;
                WayData->Top[i] = true;
                TempCut = 0;
                // Evaluate Increase Cut
                for(int j = 0; j < (int)CurrentFC->HyperGraph[i].size(); j++){
                    int NumTop = 0;
                    int NumBottom = 0;
                    for(int k = 0; k < (int)CurrentFC->NetList[CurrentFC->HyperGraph[i][j]].size(); k++){
                        if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[i][j]][k]] == true)
                            NumBottom++;
                        else
                            NumTop++;
                    }
                    if(NumTop == 1 && NumBottom != 0)
                        TempCut++;
                    else if(NumBottom == 0 && NumTop > 1)
                        TempCut--;
                }
                // decide minimum cutsizeratio
                float TempCutSize =  (float)TempCut / (float)CurrentFC->VerteciesSize[i];
                if(TempCutSize < CutSizeRatio){
                    CutSizeRatio = TempCutSize;
                    MinCutIncreaseIndex = i;
                    MinCut = TempCut;
                }
                else if(TempCutSize == CutSizeRatio && CurrentFC->HyperGraph[i].size() < CurrentFC->HyperGraph[MinCutIncreaseIndex].size()){
                    MinCutIncreaseIndex = i;
                    MinCut = TempCut;
                }
                WayData->Bottom[i] = true;
                WayData->Top[i] = false;
            }
        }
        if(MinCut == INT16_MAX)
            break;
        else{
            WayData->Bottom[MinCutIncreaseIndex] = false;
            WayData->Top[MinCutIncreaseIndex] = true;
            WayData->BottomSize -= CurrentFC->VerteciesSize[MinCutIncreaseIndex];
            WayData->TopSize += CurrentFC->VerteciesSize[MinCutIncreaseIndex];
            Cut += MinCut;
            CurrentSize -= CurrentFC->VerteciesSize[MinCutIncreaseIndex];
        }
    }

    printf("\n====================\n");
    printf("[Init Partition Result] =================================================\n");
    WayData->Cut = EvalCut(CurrentFC, WayData);
    printf("Since %3d Time Init Partition Cut is %4d                               |\n", IterationTime, WayData->Cut);
    printf("[BottomSize / (TopSize + BottomSize)] => %5d / %5d = %.5f        |\n", WayData->BottomSize, WayData->BottomSize + WayData->TopSize
    , (float)WayData->BottomSize / (float)(WayData->BottomSize + WayData->TopSize));
    printf("=========================================================================\n");

    // Let Temp way buffer equal origin way buffer
    return WayData;
}

int EvalCut(struct FCBuffer *CurrentFC, struct WayBuffer *WayData){
    int Cut = 0;
    for(int i = 0; i < (int)CurrentFC->NetList.size(); i++){
        int NumTop = 0;
        int NumBottom = 0;
        for(int j = 0; j < (int)CurrentFC->NetList[i].size(); j++){
            if(WayData->Bottom[CurrentFC->NetList[i][j]] == true)
                NumBottom++;
            else
                NumTop++;
        }
        if(NumBottom != 0 && NumTop != 0)
            Cut++;
    }
    return Cut;
}

struct WayBuffer FM(struct FCBuffer *CurrentFC, struct WayBuffer *WayData, int layer_ctr){
    // Record Time
    static int count = 0;
    // find best partition
    struct WayBuffer BestWayData = *WayData;
    // limit iteration times
    int MaxGain = INT32_MIN;
    int PreCut = -1;
    int Iteration = 0;
    int IterationTime = FMLIMIT / (count+1);
    std::vector<bool> IsVertexPass(CurrentFC->Vertecies.size(), false);
    std::vector<int> VertexGain(CurrentFC->Vertecies.size(), 0);
    // find VertexGain
    for(int i = 0; i < (int)CurrentFC->Vertecies.size(); i++){
        int Gain = 0;
        // evaluate gain => gain is old cut - new cut
        for(int j = 0; j < (int)CurrentFC->HyperGraph[i].size(); j++){
            int NumTop = 0;
            int NumBottom = 0;
            for(int k = 0; k < (int)CurrentFC->NetList[CurrentFC->HyperGraph[i][j]].size(); k++){
                if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[i][j]][k]] == true)
                    NumBottom++;
                else
                    NumTop++;
                if(NumBottom > 1 && NumTop > 1)
                    break;
            }
            // means bottom change top
            if(WayData->Bottom[i] == true){
                if(NumBottom == 1 && NumTop > 0)
                    Gain++;
                else if(NumTop == 0 && NumBottom > 1)
                    Gain--;
            }
            // means top change bottom
            else if(WayData->Top[i] == true){
                if(NumTop == 1 && NumBottom > 0)
                    Gain++;
                else if(NumBottom == 0 && NumTop > 1)
                    Gain--;
            }
        }
        VertexGain[i] = Gain;
    }
    // start FM
    while(Iteration < IterationTime){
        Iteration++;
        int MaxGainVertex;
        struct WayBuffer TempWayData = *WayData;
        MaxGain = INT32_MIN;
        // find MaxGain
        for(int i = 0; i < (int)CurrentFC->Vertecies.size(); i++){
            if(IsVertexPass[i] == false && VertexGain[i] > MaxGain){
                if(WayData->Bottom[i] == true && (float) (WayData->BottomSize-CurrentFC->VerteciesSize[i]) / (float)(WayData->BottomSize + WayData->TopSize) > Bratio &&
                  (float) (WayData->BottomSize-CurrentFC->VerteciesSize[i]) / (float)(WayData->BottomSize + WayData->TopSize) < Tratio){
                    MaxGain = VertexGain[i];
                    MaxGainVertex = i;
                }
                else if(WayData->Top[i] == true && (float) (WayData->TopSize-CurrentFC->VerteciesSize[i]) / (float)(WayData->BottomSize + WayData->TopSize) > Bratio &&
                  (float) (WayData->TopSize-CurrentFC->VerteciesSize[i]) / (float)(WayData->BottomSize + WayData->TopSize) < Tratio){
                    MaxGain = VertexGain[i];
                    MaxGainVertex = i;               
                } 
            }
            else if(IsVertexPass[i] == false && VertexGain[i] == MaxGain){
                if(WayData->Bottom[i] == true && (float) WayData->BottomSize / (float)(WayData->BottomSize + WayData->TopSize) > Mratio)
                    MaxGainVertex = i;
                else if(WayData->Top[i] == true && (float) WayData->TopSize / (float)(WayData->BottomSize + WayData->TopSize) > Mratio)
                    MaxGainVertex = i;            
            }
        }
        if(MaxGain == INT32_MIN)
            continue;
        else if(WayData->Bottom[MaxGainVertex] == true){
            WayData->Bottom[MaxGainVertex] = false;
            WayData->Top[MaxGainVertex] = true;
            // update Top and Bottom size
            WayData->BottomSize -= CurrentFC->VerteciesSize[MaxGainVertex];
            WayData->TopSize += CurrentFC->VerteciesSize[MaxGainVertex];
        }
        else{
            WayData->Bottom[MaxGainVertex] = true;
            WayData->Top[MaxGainVertex] = false;
            // update Top and Bottom size
            WayData->BottomSize += CurrentFC->VerteciesSize[MaxGainVertex];
            WayData->TopSize -= CurrentFC->VerteciesSize[MaxGainVertex];
        }
        // Is area over balance?
        if((float) WayData->BottomSize / (float)(WayData->BottomSize + WayData->TopSize) > Bratio &&
        (float) WayData->BottomSize / (float)(WayData->BottomSize + WayData->TopSize) < Tratio){
            int OrgCut = WayData->Cut;
            WayData->Cut = EvalCut(CurrentFC, WayData);
            if(WayData->Cut < BestWayData.Cut)
                BestWayData = *WayData;
            // update relate gain
            for(int i = 0; i < (int)CurrentFC->HyperGraph[MaxGainVertex].size(); i++){
                int NumTop = 0;
                int NumBottom = 0;
                for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                    if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                        NumBottom++;
                    else
                        NumTop++;
                }
                if((NumBottom == 2 && NumTop == 0) || (NumTop == 2 && NumBottom == 0)){
                    for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++)
                        VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] -= 2;
                }
                else if(NumBottom == 1 && NumTop == 1){
                    for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++)
                        VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] += 2;
                }
                else if(NumBottom == 2 && NumTop == 1){
                    if(WayData->Bottom[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                            else if(WayData->Top[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                        }
                    }
                    else{
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                        }
                    }
                }
                else if(NumTop == 2 && NumBottom == 1){
                    if(WayData->Bottom[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Top[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                        }
                    }
                    else{
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Top[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                            else if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                        }
                    }
                }
                else if((NumBottom == 0 && NumTop > 2) || (NumTop == 0 && NumBottom > 2)){
                    for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++)
                        VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                }
                else if(NumBottom == 1 && NumTop > 2){
                    if(WayData->Bottom[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++)
                            VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                    }
                    else if(WayData->Top[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                        }
                    }
                }
                else if(NumTop == 1 && NumBottom > 2){
                    if(WayData->Top[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++)
                            VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                    }
                    else if(WayData->Bottom[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Top[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]++;
                        }
                    }
                }
                else if(NumTop == 2 && NumBottom >= 2){
                    if(WayData->Top[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Top[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                        }
                    }
                    else if(NumBottom == 2){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                        }
                    }
                }
                else if(NumBottom == 2 && NumTop >= 2){
                    if(WayData->Bottom[MaxGainVertex] == true){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Bottom[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                        }
                    }
                    else if(NumTop == 2){
                        for(int j = 0; j < (int)CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]].size(); j++){
                            if(WayData->Top[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]] == true)
                                VertexGain[CurrentFC->NetList[CurrentFC->HyperGraph[MaxGainVertex][i]][j]]--;
                        }
                    }
                }
            }
        }
        else{
            // change back
            if(WayData->Bottom[MaxGainVertex] == true){
                WayData->Bottom[MaxGainVertex] = false;
                WayData->Top[MaxGainVertex] = true;
                // update Top and Bottom size
                WayData->BottomSize -= CurrentFC->VerteciesSize[MaxGainVertex];
                WayData->TopSize += CurrentFC->VerteciesSize[MaxGainVertex];
            }
            else{
                WayData->Bottom[MaxGainVertex] = true;
                WayData->Top[MaxGainVertex] = false;
                // update Top and Bottom size
                WayData->BottomSize += CurrentFC->VerteciesSize[MaxGainVertex];
                WayData->TopSize -= CurrentFC->VerteciesSize[MaxGainVertex];
            }
        }
        // update VertexPass
        IsVertexPass[MaxGainVertex] = true;
    }

    printf("\n====================\n");
    printf("[UnCoarsening %2d Times] =================================================\n", count);
    printf("Since FM %3d Times Iteration ,BestMinCut is %4d                            |\n", IterationTime, BestWayData.Cut);
    printf("[BottomSize / (TopSize + BottomSize)] => %5d / %5d = %.5f        |\n", BestWayData.BottomSize, BestWayData.BottomSize + BestWayData.TopSize
    , (float)BestWayData.BottomSize / (float)(BestWayData.BottomSize + BestWayData.TopSize));
    printf("=========================================================================\n");
    if(layer_ctr == 0)
        count = 0;
    else
        count+=SLOPE;
    return BestWayData;
}

void PrintFinalWayResult(struct WayBuffer *WayData){
    for(int i = 0; i < (int)WayData->Top.size(); i++){
        if(WayData->Top[i] == true)
            printf("Vertecies %d is Top\n", i+1);
        else
            printf("Vertecies %d is Bottom\n", i+1);  
    }
}

void ChangeWayData(struct FCBuffer *CurrentFC, struct WayBuffer *Waydata){
    std::vector<bool> Top(CurrentFC->preFC->Vertecies.size());
    std::vector<bool> Bottom(CurrentFC->preFC->Vertecies.size());
    for(int i = 0; i < (int)Waydata->Top.size(); i++){
        if(Waydata->Top[i] == true){
            for(int j = 0; j < (int)CurrentFC->Vertecies[i].size(); j++)
                Top[CurrentFC->Vertecies[i][j]] = true;
        }
        else{
            for(int j = 0; j < (int)CurrentFC->Vertecies[i].size(); j++)
                Bottom[CurrentFC->Vertecies[i][j]] = true;
        }
    }
    Waydata->Top = Top;
    Waydata->Bottom = Bottom;
}


// no FC version
struct FCBuffer *RunCoarsening(struct FCBuffer *CurrentFC, int layer_ctr){
    int TotalNetListSize = 0;
    int UpdataNetListSize;
    struct FCBuffer *nxtFC = CurrentFC->nxtFC;
    srand(time(0));
    // find HyperGraph
    std::vector<std::vector<int>> HyperGraph(CurrentFC->Vertecies.size());
    for(int i = 0; i < (int)CurrentFC->NetList.size(); i++){
        for(int j = 0; j < (int)CurrentFC->NetList[i].size(); j++){
            HyperGraph[CurrentFC->NetList[i][j]].push_back(i);
            TotalNetListSize++;
        }
    }
    CurrentFC->HyperGraph = HyperGraph;
    UpdataNetListSize = TotalNetListSize;
    printf("\n====================\n");
    printf("[Coarsening %2d Times] ===================================================\n", layer_ctr);
    printf("Layer %2d Coarsening image size is %6d                                |\n", layer_ctr, TotalNetListSize);
    // put CurrentFC verticies into vector
    std::vector<bool> MatchVertecies (CurrentFC->Vertecies.size(), false);

    // when the size of the coarsened image differs by 1.7(factor) times from the original, the n(layer_ctr)th layer of coarsening ends
    TotalNetListSize = int(TotalNetListSize / factor);
    printf("Expect Coarsening image size is %6d                                  |\n", TotalNetListSize);
    // Record Vertecies Into which set
    std::vector<int> RecordVerteciesNewPlace(CurrentFC->Vertecies.size(), 0);
    int NewVerteciesIndex = 0;
    int UnmatchVertecies = MatchVertecies.size();
    std::vector<int> AllVertecies;
    for(int i = 0; i < UnmatchVertecies; i++)
        AllVertecies.push_back(i);
    // start coarsening
    while(UpdataNetListSize > TotalNetListSize){
        // find random index
        int Index = rand() % UnmatchVertecies;
        while(MatchVertecies[AllVertecies[Index]] != false){
            UnmatchVertecies--;
            if(UnmatchVertecies <= 0)
                break;
            // swap with last Vertecies
            int temp = AllVertecies[UnmatchVertecies];
            AllVertecies[UnmatchVertecies] = AllVertecies[Index];
            AllVertecies[Index] = temp;
            Index = rand() % UnmatchVertecies;
        }
        if(UnmatchVertecies <= 0)
            break;
        else
            Index = AllVertecies[Index];
        // find one edge is min edge
        int MinCellSize = INT16_MAX;
        int MinCellSizeIndex = -1;
        for(int i = 0; i < (int)HyperGraph[Index].size(); i++){
            if((int)CurrentFC->NetList[HyperGraph[Index][i]].size() < MinCellSize){
                MinCellSize = CurrentFC->NetList[HyperGraph[Index][i]].size();
                MinCellSizeIndex = i;
            }
            else if((int)CurrentFC->NetList[HyperGraph[Index][i]].size() == MinCellSize){
                // if all unmatch than MinCellIndex first use this Vertecies
                bool is_match = false;
                for(int j = 0; j < (int)CurrentFC->NetList[HyperGraph[Index][i]].size(); j++){
                    if(MatchVertecies[CurrentFC->NetList[HyperGraph[Index][i]][j]] == true){
                        is_match = true;
                        break;
                    }
                }
                if(is_match == false)
                    MinCellSizeIndex = i;
            }
        }

        int randomHyperEdge = MinCellSizeIndex;
        // Record which Net chose
        int LossNet = 0;
        std::vector<bool> RecordLossNet(CurrentFC->NetList.size(), false);
        // Record NewVertecies
        std::vector<int> NewVertecies;
        int NewVerteciesSize = 0;
        // select edge pin all match with Index
        for(int i = 0; i < (int)CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]].size(); i++){
            if(MatchVertecies[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]] == false){
                RecordVerteciesNewPlace[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]] = NewVerteciesIndex;
                NewVertecies.push_back(CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]);
                NewVerteciesSize += CurrentFC->VerteciesSize[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]];
                MatchVertecies[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]] = true;
                UpdataNetListSize -= (int)HyperGraph[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]].size();
                // Evaluate Loss Net
                for(int j = 0; j < (int)HyperGraph[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]].size(); j++){
                    if(RecordLossNet[HyperGraph[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]][j]] == false){
                        LossNet++;
                        RecordLossNet[HyperGraph[CurrentFC->NetList[HyperGraph[Index][randomHyperEdge]][i]][j]] = true;
                    }
                }
            }
        }
        // NewVertecies push into next FC
        nxtFC->Vertecies.push_back(NewVertecies);
        // push NewVertecies size
        nxtFC->VerteciesSize.push_back(NewVerteciesSize);
        // Index++
        NewVerteciesIndex++;
        // How much will the netlist size increase if new vertex is added to the netlist?
        UpdataNetListSize += LossNet;
    }
    // if UnmatchVertecies not equal zero than most push remain vertecies to next vertecies
    while(UnmatchVertecies >= 0){
        if(MatchVertecies[AllVertecies[UnmatchVertecies]] == false){
            std::vector<int> NewVertecies;
            NewVertecies.push_back(AllVertecies[UnmatchVertecies]);
            nxtFC->Vertecies.push_back(NewVertecies);
            nxtFC->VerteciesSize.push_back(CurrentFC->VerteciesSize[AllVertecies[UnmatchVertecies]]);
            RecordVerteciesNewPlace[AllVertecies[UnmatchVertecies]] = NewVerteciesIndex;
            NewVerteciesIndex++;
            MatchVertecies[AllVertecies[UnmatchVertecies]] = true;
        }
        UnmatchVertecies--;
    }

    // make sure all origin unmatch vertecies is match
    for(int i = 0; i < (int)MatchVertecies.size(); i++){
        if(MatchVertecies[i] != true)
            printf("Vertecies %6d is not match                                           |\n", i+1);
    }

    printf("NewVertecies Size is %6d                                             |\n", (int)nxtFC->Vertecies.size());

    // udpate new netlist
    for(int i = 0; i < (int)CurrentFC->NetList.size(); i++){
        std::vector<bool> Vertecies(nxtFC->Vertecies.size(), false);
        std::vector<int> NewNet;
        int NewNetNum = 0;
        for(int j = 0; j < (int)CurrentFC->NetList[i].size(); j++){
            if(Vertecies[RecordVerteciesNewPlace[CurrentFC->NetList[i][j]]] == false){
                Vertecies[RecordVerteciesNewPlace[CurrentFC->NetList[i][j]]] = true;
                NewNetNum++;
                NewNet.push_back(RecordVerteciesNewPlace[CurrentFC->NetList[i][j]]);
            }
        }
        if(NewNetNum > 1){
            nxtFC->NetList.push_back(NewNet);
        }
    }
    printf("NewNetList Size is %5d                                                |\n", (int)nxtFC->NetList.size());

    printf("=========================================================================\n");
    return nxtFC;
}
