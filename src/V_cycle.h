#include "parse.h"

// End coarsening when the number of vertices is less than c*k
#define C 30
// k-ways
#define K 2
/* Because we use FC coarsening, when the size of the coarsened image differs by 1.7(factor) times from the original, 
 * the nth layer of coarsening ends. The size of the foul language image is equal to the total number of pins in all nets.
*/
#define factor 1.7
// because this project is two ways partition so two ways size(A and B) most satisfy Bratio < A/(A+B) < Tratio
#define Tratio 0.55 // Top ratio
#define Mratio 0.5
#define Bratio 0.45 // Bottom ratio

// By limiting the coarsening of the netlist at this stage, the size of the for loop can be used to achieve acceleration.
#define RATIO 2 // more big cut more big
#define RATIOACCELERATOR 1

// for FM paremeter if uncoarsening layer more big then FC loop more small because let program excute speed more quick
#define FMLIMIT 200
#define SLOPE 2

// how many time will bisection run?
#define Nruns 5

// Current excute case
#define CurrentCase CASE3

#define RUNS 100

struct FCBuffer{
    struct FCBuffer *preFC;
    struct FCBuffer *nxtFC;
    std::vector<int> VerteciesSize;
    std::vector<std::vector<int>> Vertecies;
    std::vector<std::vector<int>> NetList;
    std::vector<std::vector<int>> HyperGraph;
};

struct FCBuffer *RunCoarsening(struct FCBuffer *CurrentFC, int layer_ctr);
struct FCBuffer *RunFCCoarsening(struct FCBuffer *CurrentFC, int layer_ctr);
struct WayBuffer *InitPartition(struct FCBuffer *CurrentFC, int TotalSize);
struct WayBuffer FM(struct FCBuffer *CurrentFC, struct WayBuffer *WayData, int layer_ctr);
int EvalCut(struct FCBuffer *CurrentFC, struct WayBuffer *WayData);
void PrintFinalWayResult(struct WayBuffer *WayData);
void ChangeWayData(struct FCBuffer *CurrentFC, struct WayBuffer *Waydata);