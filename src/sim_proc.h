#include <queue>
#include <iostream>
#include <map>
#include <memory>
#ifndef SIM_PROC_H
#define SIM_PROC_H

typedef struct proc_params{
    unsigned long int rob_size;
    unsigned long int iq_size;
    unsigned long int width;
}proc_params;

// Put additional data structures here as per your requirement

// Parameters - sim <ROB_SIZE> <IQ_SIZE> <WIDTH>

class Reg_info {
public:
    int FE_cycle;
    int FE_duration;
    int DE_cycle;
    int DE_duration;
    int RN_cycle;
    int RN_duration;
    int RR_cycle;
    int RR_duration;
    int DI_cycle;
    int DI_duration;
    int IS_cycle;
    int IS_duration;
    int EX_cycle;
    int EX_duration;
    int WB_cycle;
    int WB_duration;
    int RT_cycle;
    int RT_duration;

    int Instruction_Num;
    int op_type;

    // Constructor
    Reg_info(int Inst_num = 0, int op_type = 0, int FE_cycle = 0, int FE_duration = 0, int DE_cycle = 0, int DE_duration = 0, int RN_cycle = 0, int RN_duration = 0, int RR_cycle = 0, 
    int RR_duration = 0, int DI_cycle = 0, int DI_duration = 0, int IS_cycle = 0, int IS_duration = 0, int EX_cycle = 0, int EX_duration = 0, 
    int WB_cycle = 0, int WB_duration = 0, int RT_cycle = 0, int RT_duration = 0) {
        this->Instruction_Num = Inst_num;
        this->op_type = op_type;
        this->FE_cycle = FE_cycle;
        this->FE_duration = FE_duration;
        this->DE_cycle = DE_cycle;
        this->DE_duration = DE_duration;
        this->RN_cycle = RN_cycle;
        this->RN_duration = RN_duration;
        this->RR_cycle = RR_cycle;
        this->RR_duration = RR_duration;
        this->DI_cycle = DI_cycle;
        this->DI_duration = DI_duration;
        this->IS_cycle = IS_cycle;
        this->IS_duration = IS_duration;
        this->EX_cycle = EX_cycle;
        this->EX_duration = EX_duration;
        this->WB_cycle = WB_cycle;
        this->WB_duration = WB_duration;
        this->RT_cycle = RT_cycle;
        this->RT_duration = RT_duration;
    }
};

class ROB {
public:
    int destination_reg;
    int latency_counter;
    bool ready; // False means havent completed yet and if you have a destination it means it not ready yet
    Reg_info reg_info;

    ROB(int destination_reg = -1, int latency_counter = 0, bool ready = false) {
        this->destination_reg = destination_reg;
        this->latency_counter = latency_counter;
        this->ready = ready;
    }
};

class pipeline_reg {
public:
    int dest;
    ROB *dest_ROB_pointer;
    int src1;
    ROB *src1_ROB_pointer;
    int src2;
    ROB *src2_ROB_pointer;
    int latency_timer; // 0 = 1 cycle, 1 = 2 cycles, 2 = 5 cycles
    int op_type;
    bool valid;
    Reg_info reg_info;
    bool retire;

    pipeline_reg(int dest = 0, ROB *dest_ROB_pointer = nullptr, int src1 = 0, ROB *src1_ROB_pointer = nullptr, int src2 = 0, 
    ROB *src2_ROB_pointer = nullptr, int op_type = 0, bool valid = false, bool retire = false) {
        this->dest = dest;
        this->dest_ROB_pointer = dest_ROB_pointer;
        this->src1 = src1;
        this->src1_ROB_pointer = src1_ROB_pointer;
        this->src2 = src2;
        this->src2_ROB_pointer = src2_ROB_pointer;
        this->op_type = op_type;
        if (op_type == 0) {
            this->latency_timer = 1;
        } else if (op_type == 1) {
            this->latency_timer = 2;
        } else if (op_type == 2) {
            this->latency_timer = 5;
        } else {
            std::cerr << "Invalid latency timer" << std::endl;
        }
        this->valid = valid;
        this->retire = retire;
    }
};

class Issue_Queue {
public:
    bool valid;
    ROB *destination_ROB_pointer; 
    bool rs1_ready;
    ROB *src1_ROB_pointer;
    bool rs2_ready;
    ROB *src2_ROB_pointer;
    int latency_timer;
    Reg_info reg_info;
    int dest;
    int src1;
    int src2;

    Issue_Queue(bool valid = false, ROB *destination_ROB_pointer = nullptr, int rs1_ready = false, ROB *src1_ROB_pointer = nullptr, 
    int rs2_ready = false, ROB *src2_ROB_pointer = nullptr, int latency_timer = 0, Reg_info reg_info = 0, int dest = -1, int src1 = -1,
    int src2 = -1)  {
        this->valid = valid;
        this->destination_ROB_pointer = destination_ROB_pointer;
        this->rs1_ready = rs1_ready;
        this->src1_ROB_pointer = src1_ROB_pointer;
        this->rs2_ready = rs2_ready;
        this->src2_ROB_pointer = src2_ROB_pointer;
        this->latency_timer = latency_timer;
        this->reg_info = reg_info;
        this->dest = dest;
        this->src1 = src1;
        this->src2 = src2;
    }
};

class Rename_Map_Table {
public:
    bool valid;
    ROB *ROB_pointer;

    // Constructor
    Rename_Map_Table(bool valid = false, ROB *ROB_pointer = nullptr){
        this->valid = valid;
        this->ROB_pointer = ROB_pointer;
    }
};

class processor{
    public:
    // Data structures
    proc_params params;
    // Pipeline register between stages with size of WIDTH
    std::vector<pipeline_reg> *DE;
    std::vector<pipeline_reg> *RN;
    std::vector<pipeline_reg> *RR;
    std::vector<pipeline_reg> *DI;

    // Issue queue with size of IQ_SIZE between Dispatch and Issue stage
    std::deque<Issue_Queue> *issue_queue;

    // Execute list represents the pipeline reg between the Issue and Execute stage, 
    // as well as all sub-pipeline stages within each function unit with a size of WIDTH * 5
    std::vector<pipeline_reg> *execute_list;

    // Writeback Buffer - pipeline reg between Execute and Writeback stage with size of WIDTH * 5
    std::vector<pipeline_reg> *WB;

    // Reorder buffer with size of ROB_SIZE 
    // Queue between the writeback and retire stage
    std::deque<ROB> *Reorder_Buffer;

    // RMT is a vector of RMT objects - there will always be 67 entries
    std::vector<Rename_Map_Table> *RMT;
    
    // Cycle Counter
    int cycle_counter;
    FILE *trace_file;
    bool end_of_file;

    // Measurements
    int Dynamic_instructions;   // Total number of retired instructions
    int cycles;                 // Total number of cycles to retire all instructions
    float IPC;                  // Dynamic instruction / cycles


    processor(proc_params params, const char *trace_filename){
        this->trace_file = fopen(trace_filename, "r");
        if (trace_file == NULL) {
            printf("Error: Unable to open file %s\n", trace_filename);
            exit(EXIT_FAILURE);
        }
        this->end_of_file = false;

        this->params = params;

        // Create memory for all pipeline registers - size of WIDTH
        DE = new std::vector<pipeline_reg>(0);
        RN = new std::vector<pipeline_reg>(0);
        RR = new std::vector<pipeline_reg>(0);
        DI = new std::vector<pipeline_reg>(0); 

        // Initialize execute list using constructors
        execute_list = new std::vector<pipeline_reg>(0);

        // Initialize Writeback Buffer using constructors
        WB = new std::vector<pipeline_reg>(0);

        // Initialize Reorder buffer - size of ROB_SIZE
        Reorder_Buffer = new std::deque<ROB>;

        // Initialize issue queue to empty - size of IQ_SIZE
        issue_queue = new std::deque<Issue_Queue>;

        // Initialize RMT
        RMT = new std::vector<Rename_Map_Table>(67);

        // Initialize all RMT entries to invalid
        for (int i = 0; i < 67; ++i){
            (*RMT)[i].valid = false;
        }

        // Initialize cycle counter
        cycle_counter = 0;

        // Initialize measurements
        Dynamic_instructions = 0;
        cycles = 0;
        IPC = 0;
    }

    // Destructor to deallocate dynamic memory
    ~processor() {
        if (trace_file != nullptr) {
            fclose(trace_file);
            trace_file = nullptr;
        }
        
        // Clear all containers
        delete DE;
        delete RN;
        delete RR;
        delete DI;
        delete issue_queue;
        delete execute_list;
        delete WB;
        delete Reorder_Buffer;
        delete RMT;
    }

    void retire(){
        // Retire up to WIDTH consecutive "ready" instructions from the head of the ROB
        if (Reorder_Buffer->size() == 0) return;
        
        for (int i = 0; i < params.width; ++i){
            if (Reorder_Buffer->front().ready){

                // Find the instruction in WB
                for (int j = 0; j < WB->size(); j++){
                    if (WB->at(j).dest_ROB_pointer == &Reorder_Buffer->front()){ // Found matching ROB entry

                        // Print the retire instruction
                        this->print_measurements(WB->at(j));

                        // Remove WB entry
                        WB->erase(WB->begin() + j);
                    }
                }
                // Reove ROB entry
                Reorder_Buffer->pop_front();
            }
        }
    }

    void writeback(){
        // Process the writeback bundle from WB: for each instruction in WB, mark the instruction as ready in its entry in the ROB
        for (int i = 0; i < WB->size(); ++i){
            WB->at(i).dest_ROB_pointer->ready = true;
            WB->at(i).retire = true;
        }
    }

    void execute(){
        // From execute list, check for instructions that are finishing execution this cycle, and:
        // 1) Remove the instruction from the execute list
        // 2) Add the instruction to WB.
        // 3) Wakeup dependent instructions (set their source operand ready flags to true) in the IQ, DI, and RR.

        if (!execute_list->empty()){
            for (int i = 0; i < execute_list->size(); ++i){
                if (execute_list->at(i).latency_timer <= 0){
                    WB->emplace_back(execute_list->at(i));
                    execute_list->erase(execute_list->begin() + i);

                    // Wakeup dependent instructions (set their source operand ready flags to true) in the IQ, DI, and RR.
                    // IQ
                    // If the destination at WB->back() is a source in IQ, update it to ready
                    for (auto &iq_entry : *issue_queue) {
                        // Check if src1 depends on the completing instruction
                        if (iq_entry.src1 == WB->back().dest) {
                            iq_entry.rs1_ready = true;
                        }
                        // Check if src2 depends on the completing instruction
                        if (iq_entry.src2 == WB->back().dest) {
                            iq_entry.rs2_ready = true;
                        }
                    }

                    // Wake up dependent instructions in DI
                    for (auto &di_entry : *DI) {
                        if (di_entry.src1 == WB->back().dest && di_entry.src1_ROB_pointer != nullptr) {
                            di_entry.src1_ROB_pointer->ready = true;
                        }
                        if (di_entry.src2 == WB->back().dest && di_entry.src2_ROB_pointer != nullptr) {
                            di_entry.src2_ROB_pointer->ready = true;
                        }
                    }

                    // Wake up dependent instructions in RR
                    for (auto &rr_entry : *RR) {
                        if (rr_entry.src1 == WB->back().dest && rr_entry.src1_ROB_pointer != nullptr) {
                            rr_entry.src1_ROB_pointer->ready = true;
                        }
                        if (rr_entry.src2 == WB->back().dest && rr_entry.src2_ROB_pointer != nullptr) {
                            rr_entry.src2_ROB_pointer->ready = true;
                        }
                    }
                }
            }
        }
    }

    void issue(){
        // Issue up to WIDTH oldest instructions from the IQ (IQ is a deque so oldest instructions are at the front)
        // To issue an instruction, Remove the instruction from the front of the IQ and add it to the Execute list
        // Set a timer for the instruction in the execute list that will model the latency of the instruction
        for (int i = 0; i < params.width; ++i){
            if (issue_queue->empty()){
                break;
            } else if (issue_queue->at(i).rs1_ready && issue_queue->at(i).rs2_ready){   // Check if RS1 and RS2 are ready in issue queue - if not skip
                // Create local variables to avoid dereferencing null pointers
                ROB* temp_dest_ROB_pointer = (issue_queue->at(i).destination_ROB_pointer != nullptr) ? issue_queue->at(i).destination_ROB_pointer : nullptr;
                ROB* temp_src1_ROB_pointer = (issue_queue->at(i).src1_ROB_pointer != nullptr) ? issue_queue->at(i).src1_ROB_pointer : nullptr;
                ROB* temp_src2_ROB_pointer = (issue_queue->at(i).src2_ROB_pointer != nullptr) ? issue_queue->at(i).src2_ROB_pointer : nullptr;
                bool temp_rs1_ready = (temp_src1_ROB_pointer != nullptr) ? temp_src1_ROB_pointer->ready : true;
                bool temp_rs2_ready = (temp_src2_ROB_pointer != nullptr) ? temp_src2_ROB_pointer->ready : true;
                int temp_dest_reg = issue_queue->at(i).dest;
                int temp_src1_reg = issue_queue->at(i).src1;
                int temp_src2_reg = issue_queue->at(i).src2;
                int temp_latency_timer = issue_queue->at(i).latency_timer;

                execute_list->emplace_back(temp_dest_reg, temp_dest_ROB_pointer, temp_src1_reg, temp_src1_ROB_pointer, 
                temp_src2_reg, temp_src2_ROB_pointer);   

                // Update object fields that arent copied correctly
                execute_list->back().reg_info = issue_queue->at(i).reg_info; 
                execute_list->back().latency_timer = temp_latency_timer;

                issue_queue->pop_front();
            }
        }
    }

    void dispatch(){
        // If DI contains a dispatch bundle:
        if (!DI->empty()){
                // If the number of free IQ entries is less than the size of the dispatch bundle, then do nothing.
            if (issue_queue->size() + DI->size() > params.iq_size){
                return;
            } else { // else dispatch all instructions from DI to the IQ
                for (int i = 0; i < DI->size(); ++i){
                    ROB* temp_dest_ROB_pointer = ((*DI)[i].dest_ROB_pointer != nullptr) ? (*DI)[i].dest_ROB_pointer : nullptr;
                    ROB* temp_src1_ROB_pointer = ((*DI)[i].src1_ROB_pointer != nullptr) ? (*DI)[i].src1_ROB_pointer : nullptr;
                    ROB* temp_src2_ROB_pointer = ((*DI)[i].src2_ROB_pointer != nullptr) ? (*DI)[i].src2_ROB_pointer : nullptr;
                    int temp_latency_timer = (*DI)[i].latency_timer;
                    bool temp_rs1_ready = (temp_src1_ROB_pointer != nullptr) ? temp_src1_ROB_pointer->ready : true;
                    bool temp_rs2_ready = (temp_src2_ROB_pointer != nullptr) ? temp_src2_ROB_pointer->ready : true;

                    issue_queue->emplace_back(true, temp_dest_ROB_pointer, temp_rs1_ready, 
                    temp_src1_ROB_pointer, temp_rs2_ready, temp_src2_ROB_pointer, temp_latency_timer, (*DI)[i].reg_info, (*DI)[i].dest,
                    (*DI)[i].src1, (*DI)[i].src2);
                }
                DI->clear();
            }
        }
    }

    // Reg read handles latency of ROB entries to set them as ready and 
    // wakes up dependent operands not just in the IQ but also in the other two stages including regRead()
    void regRead(){
        // If RR contains a regRead bundle:
            // If DI is not empty, then do nothing.
        if (!RR->empty()){
            if (!DI->empty()){
                return;
            } else if (DI->empty()){ // If DI is empty, then process the regRead bundle from RR to DI
                *DI = *RR;
                RR->clear();
            }
        }
    }

    void rename(){
        // If RN contains a rename bundle:
            // If either RR is not empty or the ROB does not have enough free entries to accept the entire rename bundle, then do nothing.
        if (!RN->empty()){
            if (!RR->empty() || Reorder_Buffer->size() + RN->size() > params.rob_size){
                return;
            // If RR is empty and the ROB has enough free entries to accept the entire rename bundle, then:
            } else if (RR->empty() && Reorder_Buffer->size() + RN->size() < params.rob_size){
                for (int i = 0; i < RN->size(); ++i){
                    // (1) allocate an entry in the ROB for the destination - grab the most recent version of the source registers from the RMT
                    // Check if the destination is invalid (-1)
                    if ((*RN)[i].dest != -1){
                        Reorder_Buffer->push_back(ROB((*RN)[i].dest, (*RN)[i].latency_timer, false));           // Add destination to the ROB
                        (*RMT)[(*RN)[i].dest].valid = true;                            // Set RMT entry to valid
                        (*RMT)[(*RN)[i].dest].ROB_pointer = &Reorder_Buffer->back();   // Set RMT entry to point to the ROB entry
                        (*RN)[i].dest_ROB_pointer = &Reorder_Buffer->back();           // Add ROB pointer to instruction pipeline register
                    } else {
                        Reorder_Buffer->push_back(ROB(-1, (*RN)[i].latency_timer, false));
                        (*RN)[i].dest_ROB_pointer = &Reorder_Buffer->back();
                    }

                    // (2) rename source registers by looking in the RMT to see if there are in any flight producers of this register
                    if ((*RMT)[(*RN)[i].src1].valid){
                        (*RN)[i].src1_ROB_pointer = (*RMT)[(*RN)[i].src1].ROB_pointer;
                    }
                    if ((*RMT)[(*RN)[i].src2].valid){
                        (*RN)[i].src2_ROB_pointer = (*RMT)[(*RN)[i].src2].ROB_pointer;
                    }
                }

                // Advance instructions in RN to RR
                *RR = *RN;
                RN->clear();
            }
        }
    }

    void decode(){
        // If DE contains a decode bundle: 
            // If RN is not empty, then do nothing. 
            // else (RN is empty), then advance the decode bundle from DE to RN.
        if (!DE->empty()){
            if (!RN->empty()){
                return;
            } else {
                *RN = *DE;
                DE->clear();
            }
        }
    }

    void fetch(){
        uint64_t pc;
        int op_type;
        int dest;
        int src1;
        int src2;
        // Do nothing if either (1) there are no more instructions in the trace file or
        // (2) DE is not empty (cannot accept a new decode bundle).
        if (end_of_file || !DE->empty()) {
            return;
        }

        // If there are more instructions in the trace file and if the DE is empty, then fetch up to WIDTH instructions from the trace file into DE.
        for (int i = 0; i < params.width; ++i){
            // Make room for the new instruction
            if (fscanf(trace_file, "%llx %d %d %d %d", &pc, &op_type, &dest, &src1, &src2) == EOF) {
                end_of_file = true;
                break;
            }
            // Create new instruction in DE pipeline register
            (*DE).emplace_back(dest, nullptr, src1, nullptr, src2, nullptr, op_type, false);
            (*DE).back().reg_info.FE_cycle = cycle_counter;
            (*DE).back().reg_info.FE_duration++;
            (*DE).back().reg_info.Instruction_Num = Dynamic_instructions;
            (*DE).back().reg_info.op_type = op_type;
            Dynamic_instructions++;
        }
    }

    bool Advance_Cycle(){

        cycle_counter++;

        // Update register info in each pipeline stage
        // Stage 1: Fetch -> Decode
        for (int i = 0; i < DE->size(); ++i){
            if (DE->at(i).reg_info.DE_cycle == 0){
                DE->at(i).reg_info.DE_cycle = cycle_counter;
            }
            DE->at(i).reg_info.DE_duration++;
        }

        // Stage 2: Decode -> Rename
        for (int i = 0; i < RN->size(); ++i){
            if (RN->at(i).reg_info.RN_cycle == 0){
                RN->at(i).reg_info.RN_cycle = cycle_counter;
            }
            RN->at(i).reg_info.RN_duration++;
        }
        
        // Stage 3: Rename -> Register Read
        for (int i = 0; i < RR->size(); ++i){
            if (RR->at(i).reg_info.RR_cycle == 0){
                RR->at(i).reg_info.RR_cycle = cycle_counter;
            }
            RR->at(i).reg_info.RR_duration++;
        }

        // Stage 4: Register Read -> Dispatch
        for (int i = 0; i < DI->size(); ++i){
            if (DI->at(i).reg_info.DI_cycle == 0){
                DI->at(i).reg_info.DI_cycle = cycle_counter;
            }
            DI->at(i).reg_info.DI_duration++;
        }

        // Stage 5: Dispatch -> Issue
        for (int i = 0; i < issue_queue->size(); ++i){
            if (issue_queue->at(i).reg_info.IS_cycle == 0){
                issue_queue->at(i).reg_info.IS_cycle = cycle_counter;
            }
            issue_queue->at(i).reg_info.IS_duration++;
        }

        // Stage 6: Issue -> Execute
        for (int i = 0; i < execute_list->size(); ++i){
            if (execute_list->at(i).reg_info.EX_cycle == 0){
                execute_list->at(i).reg_info.EX_cycle = cycle_counter;
            }
            execute_list->at(i).reg_info.EX_duration++;
        }

        // Stage 7: Execute -> Writeback
        for (int i = 0; i < WB->size(); ++i){
            if(WB->at(i).retire == true){
                if(WB->at(i).reg_info.RT_cycle == 0) WB->at(i).reg_info.RT_cycle = cycle_counter;
                WB->at(i).reg_info.RT_duration++;
            } else {
                if (WB->at(i).reg_info.WB_cycle == 0) WB->at(i).reg_info.WB_cycle = cycle_counter;
                WB->at(i).reg_info.WB_duration++;
            }    
        }

        // Decrement all execute list entries latency counters
        for (int i = 0; i < execute_list->size(); ++i){
            execute_list->at(i).latency_timer = (execute_list->at(i).latency_timer > 0) ? execute_list->at(i).latency_timer - 1 : 0;
        }

        // Check if all instructions have left the retire stage
        if (Reorder_Buffer->empty() && end_of_file){
            return false;
        } else {
            return true;
        }
    }

    void print_measurements(pipeline_reg &inst){
        // <seq_no> fu{<op_type>} src{<src1>,<src2>} dst{<dst>}
        // FE{<begin-cycle>,<duration>} DE{…} RN{…} RR{…} DI{…} IS{…} EX{…}
        // WB{…} RT{…}

        std::cout << inst.reg_info.Instruction_Num << " fu{" << inst.reg_info.op_type << "} src{" << inst.src1 << "," << inst.src2 << "} dst{" << inst.dest << "}" 
        << " FE{" << inst.reg_info.FE_cycle << "," << inst.reg_info.FE_duration << "} DE{" << inst.reg_info.DE_cycle << "," << inst.reg_info.DE_duration << "}"
        << " RN{" << inst.reg_info.RN_cycle << "," << inst.reg_info.RN_duration << "} RR{" << inst.reg_info.RR_cycle << "," << inst.reg_info.RR_duration << "}"
        << " DI{" << inst.reg_info.DI_cycle << "," << inst.reg_info.DI_duration << "} IS{" << inst.reg_info.IS_cycle << "," << inst.reg_info.IS_duration << "}"
        << " EX{" << inst.reg_info.EX_cycle << "," << inst.reg_info.EX_duration << "} WB{" << inst.reg_info.WB_cycle << "," << inst.reg_info.WB_duration << "}"
        << " RT{" << inst.reg_info.RT_cycle << "," << inst.reg_info.RT_duration << "}" << std::endl;
    }
};

#endif
