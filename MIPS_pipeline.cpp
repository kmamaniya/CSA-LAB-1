#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC=0;
    bool        nop = false;  
};

struct IDStruct {
    bitset<32>  Instr=0;
    bool        nop = true;  
};

struct EXStruct {
    bitset<32>  Read_data1=0;
    bitset<32>  Read_data2=0;
    bitset<16>  Imm=0;
    bitset<5>   Rs=0;
    bitset<5>   Rt=0;
    bitset<5>   Wrt_reg_addr=0;
    bool        is_I_type=false;
    bool        rd_mem=false;
    bool        wrt_mem=false; 
    bool        alu_op=true;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable=false;
    bool        nop = true;  
};


struct MEMStruct {
    bitset<32>  ALUresult=0;
    bitset<32>  Store_data=0;
    bitset<5>   Rs=0;
    bitset<5>   Rt=0;    
    bitset<5>   Wrt_reg_addr=0;
    bool        rd_mem=false;
    bool        wrt_mem=false; 
    bool        wrt_enable=false;    
    bool        nop = true;    
};

struct WBStruct {
    bitset<32>  Wrt_data=0;
    bitset<5>   Rs=0;
    bitset<5>   Rt=0;     
    bitset<5>   Wrt_reg_addr=0;
    bool        wrt_enable=false;
    bool        nop = true;     
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public: 
        bitset<32> Reg_data;
     	RF()
    	{ 
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr)
        {   
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
		 
		void outputRF()
		{
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF:\t"<<endl;
				for (int j = 0; j<32; j++)
				{        
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;	
};

class INSMem
{
	public:
        bitset<32> Instruction;
        INSMem()
        {       
			IMem.resize(MemSize); 
            ifstream imem;
			string line;
			int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{      
					IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open file";
			imem.close();                     
		}
                  
		bitset<32> readInstr(bitset<32> ReadAddress) 
		{    
			string insmem;
			insmem.append(IMem[ReadAddress.to_ulong()].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
			insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
			Instruction = bitset<32>(insmem);		//read instruction memory
			return Instruction;     
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public:
        bitset<32> ReadData;  
        DataMem()
        {
            DMem.resize(MemSize); 
            ifstream dmem;
            string line;
            int i=0;
            dmem.open("dmem.txt");
            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {      
                    DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address)
        {	
			string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);		//read data memory
            return ReadData;               
		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)            
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));  
        }   
                     
        void outputDataMem()
        {
            ofstream dmemout;
            dmemout.open("dmemresult.txt");
            if (dmemout.is_open())
            {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open file";
            dmemout.close();               
        }             
      
    private:
		vector<bitset<8> > DMem;      
};  

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl; 
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;        
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;        
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 
//--------------DECODE THE INSTRUCTION TYPE-------------------------------------

int decodeType(bitset<32> instr){
	switch(((instr.to_ulong()>>26) & 0x3F)){

		case 0: return 0;
		case 2: return 2;
		case 3: return 2;
		default: return 1;
	}
}

//---------------------------------------------------------------------

unsigned long signExtention(bitset<16> Imm){
	if(Imm[15]==true){
		return (Imm.to_ulong()+0xFFFF0000);
	}	
	else return Imm.to_ulong();
}

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    stateStruct state,newState;
    int cycle=0;         
    while (1) {

    if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
    	break;

        /* --------------------- WB stage --------------------- */
	if(state.WB.nop==false){
		//cout<<"WB"<<endl;
		if(state.WB.wrt_enable == true){
			myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);
		}
	}


        /* --------------------- MEM stage --------------------- */
    if(state.MEM.nop==false){
		newState.WB.wrt_enable = state.MEM.wrt_enable;
		newState.WB.nop=state.MEM.nop;
		newState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;

		newState.WB.Rs = state.MEM.Rs;
		newState.WB.Rt = state.MEM.Rt;
		
		
		//--------- I - Type -------------------
		newState.WB.Wrt_data = state.MEM.ALUresult;	
		if((state.WB.wrt_enable == true) && (state.MEM.Rt == state.WB.Wrt_reg_addr) && state.WB.nop==false){			// SW-LW
				//newState.MEM.Store_data = newState.WB.Wrt_data;				
				state.MEM.Store_data = state.WB.Wrt_data;	
				cout<<"\t\tSW MEM to MEM"<<endl;
			}

		//------- LW -------------------
		if(state.MEM.rd_mem == true){
			newState.WB.Wrt_reg_addr = state.MEM.Rt;
 			newState.WB.Wrt_data = myDataMem.readDataMem(state.MEM.ALUresult);
		}

		//------ SW ----------------------
		else if(state.MEM.wrt_mem == true){
			newState.WB.Wrt_data = state.MEM.Store_data;	
			myDataMem.writeDataMem(state.MEM.ALUresult,myRF.readRF(state.MEM.Rt)); 
		}
		//cout<<"MEM"<<endl;
	}
	else
		newState.WB.nop=true;


        /* --------------------- EX stage --------------------- */
    if(state.EX.nop==false){
		newState.MEM.nop=state.EX.nop;
		newState.MEM.wrt_enable = state.EX.wrt_enable;
		newState.MEM.Rs = state.EX.Rs;
		newState.MEM.Rt = state.EX.Rt;
		newState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;

		newState.MEM.wrt_mem = state.EX.wrt_mem;
		newState.MEM.rd_mem = state.EX.rd_mem;
		//cout<<"EX"<<endl;
		if(state.EX.is_I_type == false){
				
			if((state.WB.wrt_enable == true)&&(state.WB.nop==false)){
				if(state.EX.Rs == state.WB.Wrt_reg_addr){	
						state.EX.Read_data1 = state.WB.Wrt_data;
						cout<<"\t\tADD - MEM to EX"<<endl;
				}
				if(state.EX.Rt == state.WB.Wrt_reg_addr){
						state.EX.Read_data2 = state.WB.Wrt_data;
						cout<<"\t\tADD - MEM to EX"<<endl;
				}
			}	
			
			if((state.MEM.wrt_enable == true)&&(state.MEM.nop==false)){
				if(state.EX.Rt == state.MEM.Wrt_reg_addr){
					state.EX.Read_data2 = state.MEM.ALUresult;
					cout<<"\t\tADD - EX to EX"<<endl;
				}
				
				if(state.EX.Rs == state.MEM.Wrt_reg_addr){
					state.EX.Read_data1 = state.MEM.ALUresult;
					cout<<"\t\tADD - EX to EX"<<endl;
				}		
				
			}


			if(state.EX.alu_op==true)
				newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
			else newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong();
			newState.MEM.Store_data = state.EX.Read_data2;
		}	


		else{
			newState.MEM.Store_data = state.EX.Read_data2;
			if(state.EX.rd_mem == false && state.EX.wrt_mem == false)
				newState.MEM.nop == true;
			else{
				if((state.MEM.wrt_enable == true)&&(state.MEM.nop==false) &&(state.EX.Rs == state.MEM.Wrt_reg_addr)){
										// LOAD-ADD
					state.EX.Read_data1 = state.WB.Wrt_data;
					cout<<"\t\tLOAD MEM to EX1"<<endl;
								
				}
				if((state.WB.wrt_enable == true)&&(state.WB.nop==false)&& (state.EX.Rs == state.WB.Wrt_reg_addr)){
					
										//LOAD-ADD
						state.EX.Read_data1 = state.MEM.ALUresult;
						cout<<"\t\tLOAD EX to EX"<<endl;
				}		
					
				newState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + signExtention(state.EX.Imm.to_ulong());	
				
			}
		}
	}
	else
		newState.MEM.nop=true;
          

        /* --------------------- ID stage --------------------- */
	if(state.ID.nop==false){
		newState.EX.nop=state.ID.nop;
		//cout<<"ID"<<endl;
		switch(decodeType(state.ID.Instr)){
					// R - TYPE
			case 0:
				if((state.ID.Instr.to_ulong()&0x3F)==0x23){
					cout<<"SUBU"<<endl;
					newState.EX.alu_op = false;
				}
				else{
					cout<<"ADDU"<<endl; 
					newState.EX.alu_op = true;
				}
				newState.EX.Wrt_reg_addr = ((state.ID.Instr.to_ulong()>>11)&0x1F);
				newState.EX.Rt = ((state.ID.Instr.to_ulong()>>16)&0x1F);
				newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
				newState.EX.Rs = ((state.ID.Instr.to_ulong()>>21)&0x1F);
				newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
				newState.EX.is_I_type = false;
				newState.EX.wrt_enable = true;
				newState.EX.wrt_mem = false;
				newState.EX.rd_mem = false;
				newState.EX.Imm = ((state.ID.Instr.to_ulong()&0xFFFF));

				if(state.EX.rd_mem == true && ((state.EX.Wrt_reg_addr == newState.EX.Rt)||(state.EX.Wrt_reg_addr == newState.EX.Rt))){
					cout<<"\t\tSTALL"<<endl;
					newState.ID=state.ID;
					newState.IF=state.IF;
					newState.EX.nop = true;
					printState(newState, cycle++);
					state=newState;
					continue;
				}
				break;

					// I - TYPE
			case 1: 	
				newState.EX.is_I_type = true;				
				newState.EX.alu_op = true;
				newState.EX.Imm = ((state.ID.Instr.to_ulong()&0xFFFF));
				newState.EX.Rt = ((state.ID.Instr.to_ulong()>>16)&0x1F);
				newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
				newState.EX.Rs = ((state.ID.Instr.to_ulong()>>21)&0x1F);
				newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
				
				if(((state.ID.Instr.to_ulong()>>26) & 0x3F) == 0x2B){
					cout<<"SW"<<endl;
					newState.EX.wrt_mem = true;
					newState.EX.rd_mem = false;
					newState.EX.wrt_enable = false;
					newState.EX.Wrt_reg_addr = ((state.ID.Instr.to_ulong()>>16)&0x1F);
				}
				else if(((state.ID.Instr.to_ulong()>>26) & 0x3F) == 0x23){ 		
					cout<<"LW"<<endl;
					newState.EX.wrt_mem = false;
					newState.EX.rd_mem = true;
					newState.EX.wrt_enable = true;
					newState.EX.Wrt_reg_addr = ((state.ID.Instr.to_ulong()>>16)&0x1F);
				}
				else {  					
					cout<<"BEQ"<<endl;
					newState.EX.wrt_mem = false;
					newState.EX.rd_mem = false;
					newState.EX.wrt_enable = false;
					newState.EX.Wrt_reg_addr = ((state.ID.Instr.to_ulong()>>16)&0x1F);
					if(newState.EX.Read_data1 != newState.EX.Read_data2){
						newState.IF.PC = (unsigned long)((signExtention(newState.EX.Imm)<<2)+state.IF.PC.to_ulong());
						state.IF.nop = true;
						newState.EX.nop = true;
						//cout<<cycle<<endl;
        				///printState(newState, cycle++); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       					//state = newState;
						//continue;
					}
				}
				break;
			
			/*		// J - TYPE
			case 2: 
				opc = 2;
				add = (instr.to_ulong() & 0x3FFFFFF);
				break;*/
			default: break;		
		}
	}
	else
		newState.EX.nop=true;
        
        /* --------------------- IF stage --------------------- */
	if(state.IF.nop==false){		
		newState.ID.nop = state.IF.nop;		
		//cout<<"IF"<<endl;
		newState.ID.Instr=myInsMem.readInstr(state.IF.PC);	// Instruction passed to next stage
		newState.IF.PC = state.IF.PC.to_ulong()+4;		// PC updation
		if(myInsMem.Instruction == 0xFFFFFFFF){
			newState.IF = state.IF;
			newState.IF.nop = true;
			newState.ID.nop=true;
			//cout<<"HALT"<<endl;
		}
		//cout<<myInsMem.Instruction<<endl;
	}
	else
		newState.ID.nop=true;					// NOP propagation
	     

        cout<<cycle<<endl;
        printState(newState, cycle++); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */ 
                	
    }
    
    myRF.outputRF(); // dump RF;	
	myDataMem.outputDataMem(); // dump data mem 
	
	return 0;
}
