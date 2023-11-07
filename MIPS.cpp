#include <bitset>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define ADDU (1)
#define SUBU (3)
#define AND (4)
#define OR (5)
#define NOR (7)

// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)

class RF
{
  public:
    bitset<32> ReadData1, ReadData2;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<32>(0);
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {
        /**
         * @brief Reads or writes data from/to the Register.
         *
         * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
         * Put the read results to the ReadData1 and ReadData2.
         */
        // TODO: implement!

        ReadData1 = bitset<32>(0);
        ReadData2 = bitset<32>(0);

        // Reading Register 1 (RdReg1)
        long ReadRegisterNumber1 = RdReg1.to_ulong();
        ReadData1 = Registers[ReadRegisterNumber1];

        // Reading Register 2 (RdReg2)
        long ReadRegisterNumber2 = RdReg2.to_ulong();
        ReadData2 = Registers[ReadRegisterNumber2];

        // Writing to Write Register (WrtReg)
        if (WrtEnable == 1)
        {
            long WriteRegisterNumber = WrtReg.to_ulong();
            Registers[WriteRegisterNumber] = WrtData;
        }
    }

    void OutputRF()
    {
        ofstream rfout;
        rfout.open("RFresult.txt", std::ios_base::app);
        if (rfout.is_open())
        {
            rfout << "A state of RF:" << endl;
            for (int j = 0; j < 32; j++)
            {
                rfout << Registers[j] << endl;
            }
        }
        else
            cout << "Unable to open file";
        rfout.close();
    }

  private:
    vector<bitset<32> > Registers;
};

class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {
        /**
         * @brief Implement the ALU operation here.
         *
         * ALU operation depends on the ALUOP, which are definded as ADDU, SUBU, etc.
         */
        // TODO: implement!
        ALUresult = bitset<32>(0);
        long ALUOPValue = ALUOP.to_ulong();
        if (ALUOP == ADDU)
        {
            ALUresult = bitset<32>(oprand1.to_ulong() + oprand2.to_ulong());
        }
        else if (ALUOP == SUBU)
        {
            ALUresult = bitset<32>(oprand1.to_ulong() - oprand2.to_ulong());
        }
        else if (ALUOP == AND)
        {
            ALUresult = oprand1 & oprand2;
        }
        else if (ALUOP == OR)
        {
            ALUresult = oprand1 | oprand2;
        }
        else if (ALUOP == NOR)
        {
            string operand_string_1 = oprand1.to_string().substr(32 - (int)(log2(oprand1.to_ulong())) - 1);
            string operand_string_2 = oprand2.to_string().substr(32 - (int)(log2(oprand2.to_ulong())) - 1);
            operand_string_1 = string(32 - (int)log2(oprand1.to_ulong()) - 1, '1') + operand_string_1;
            operand_string_2 = string(32 - (int)log2(oprand2.to_ulong()) - 1, '1') + operand_string_2;
            // cout << operand_string_1 << "\n";
            // cout << operand_string_2 << "\n";
            ALUresult = ~(bitset<32>(operand_string_1) | bitset<32>(operand_string_2));
        }

        return ALUresult;
    }
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
        int i = 0;
        imem.open("imem.txt");
        if (imem.is_open())
        {
            while (getline(imem, line))
            {
                IMem[i] = bitset<8>(line);
                i++;
            }
        }
        else
            cout << "Unable to open file";
        imem.close();
    }

    bitset<32> ReadMemory(bitset<32> ReadAddress)
    {
        // TODO: implement!
        /**
         * @brief Read Instruction Memory (IMem).
         *
         * Read the byte at the ReadAddress and the following three byte,
         * and return the read result.
         */
        Instruction = bitset<32>(0);
        for (int i = 0; i < 4; i++)
        {
            bitset<8> InstructionPart = IMem[ReadAddress.to_ulong() + i];
            Instruction |= (InstructionPart.to_ulong() << (8 * (3 - i)));
        }
        return Instruction;
    }

  private:
    vector<bitset<8> > IMem;
};

class DataMem
{
  public:
    bitset<32> readdata;
    DataMem()
    {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i = 0;
        dmem.open("dmem.txt");
        if (dmem.is_open())
        {
            while (getline(dmem, line))
            {
                DMem[i] = bitset<8>(line);
                i++;
            }
        }
        else
            cout << "Unable to open file";
        dmem.close();
    }
    bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
    {
        /**
         * @brief Reads/writes data from/to the Data Memory.
         *
         * This function is used to read/write data from/to the DataMem, depending on the readmem and writemem.
         * First, if writemem enabled, WriteData should be written to DMem, clear or ignore the return value readdata,
         * and note that 32-bit WriteData will occupy 4 continious Bytes in DMem.
         * If readmem enabled, return the DMem read result as readdata.
         */
        // TODO: implement!
        long AddressValue = Address.to_ulong();
        if (readmem == 1)
        {
            readdata = bitset<32>(0); // Adding this to avoid reading garbage values...
            for (int i = 0; i < 4; i++)
            {
                bitset<8> DataPart = DMem[AddressValue + i];
                readdata |= (DataPart.to_ulong() << (8 * (3 - i)));
            }
        }
        if (writemem == 1)
        {
            int j = 0;
            for (int i = 0; i < 32; i = i + 8)
            {
                bitset<8> DataPart = bitset<8>(WriteData.to_string().substr(i, 8));
                // cout << "Data Part        : " << DataPart << endl;
                DMem[AddressValue + j] = DataPart;
                j++;
            }
            readdata = bitset<32>(0);
        }
        return readdata;
    }

    void OutputDataMem()
    {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open())
        {
            for (int j = 0; j < 1000; j++)
            {
                dmemout << DMem[j] << endl;
            }
        }
        else
            cout << "Unable to open file";
        dmemout.close();
    }

  private:
    vector<bitset<8> > DMem;
};

bitset<32> incrementBy(bitset<32> number, int incrementValue)
{
    long numberValue = number.to_ulong();
    bitset<32> result(numberValue + incrementValue);
    return result;
}

bitset<32> signExtend(bitset<16> number)
{
    bitset<32> signExtendedNumber(0);
    bool isNegative = number[15];
    if (isNegative)
    {
        signExtendedNumber = bitset<32>(string(16, '1') + number.to_string());
    }
    else
    {
        signExtendedNumber = bitset<32>(string(16, '0') + number.to_string());
    }
    return signExtendedNumber;
}

int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    bitset<32> programCounter(0);
    bitset<32> Instruction(0);

    char InstructionType;
    string InstructionString;
    string OpCode;

    string rType("000000");      // OP CODE = 0x00
    string jType("000010");      // OP CODE = 0x02
    string loadType("100011");   // OP CODE = 0x23
    string storeType("101011");  // OP CODE = 0x2B
    string branchType("000100"); // OP CODE = 0x04
    string haltType("111111");   // Only for the simulation

    bitset<3> ALUOP(0);

    bitset<1> WrtEnable(0);
    bitset<5> RdReg1(0);
    bitset<5> RdReg2(0);
    bitset<5> WrtReg(0);
    bitset<32> WrtData(0);

    bitset<5> ShiftAmt(0);
    bitset<6> FunctionCode(0);

    bitset<32> Address(0);
    bitset<1> readmem(0);
    bitset<1> writemem(0);

    bitset<16> imm(0);
    bitset<1> isEqual(0);

    bitset<32> jumpAddress(0);

    while (1) // TODO: implement!
    {
        // Initialising the Program Counter...
        cout << "" << endl;
        cout << "Program Counter  : " << programCounter << " (" << programCounter.to_ulong() << ")" << endl;

        // Fetching an Instruction from Instruction Memory...
        Instruction = myInsMem.ReadMemory(programCounter);
        cout << "Instruction      : " << Instruction << endl;

        // Converting Instruction to String form for easier decoding...
        InstructionString = Instruction.to_string();

        // Extracting the OP Code from the Instruction String...
        OpCode = InstructionString.substr(0, 6);

        // Finding the Instruction Type based on the Op Code...
        if (OpCode == jType)
        {
            InstructionType = 'J';
        }
        else if (OpCode == rType)
        {
            InstructionType = 'R';
        }
        else if (OpCode == loadType)
        {
            InstructionType = 'L'; // Subtype of I-Type Instruction
        }
        else if (OpCode == storeType)
        {
            InstructionType = 'S'; // Subtype of I-Type Instruction
        }
        else if (OpCode == branchType)
        {
            InstructionType = 'B'; // Subtype of I-Type Instruction
        }
        else if (OpCode == haltType)
        {
            cout << "Program Terminated!" << endl;
            break;
        }
        else
            InstructionType = 'I';
        cout << "Instruction Type : " << InstructionType << endl;

        cout << "" << endl;
        cout << "- Initialisation -" << endl;
        // ----------------------
        //  INITIALIZATION PHASE
        // ----------------------
        if (InstructionType == 'R')
        {
            // Initialization Logic for R-type Instructions
            RdReg1 = bitset<5>(InstructionString.substr(6, 5));
            cout << "Read Register 1  : " << RdReg1 << endl;
            RdReg2 = bitset<5>(InstructionString.substr(11, 5));
            cout << "Read Register 2  : " << RdReg2 << endl;
            WrtReg = bitset<5>(InstructionString.substr(16, 5));
            cout << "Write Register   : " << WrtReg << endl;
            ShiftAmt = bitset<5>(InstructionString.substr(21, 5));
            cout << "Shift Amount     : " << ShiftAmt << endl;
            FunctionCode = bitset<6>(InstructionString.substr(26));
            cout << "Function Code    : " << FunctionCode << endl;
            ALUOP = bitset<3>(InstructionString.substr(29));
            cout << "ALUOP            : " << ALUOP << endl;
            // For R-Type Initialisation, WrtData is 0
            WrtData = bitset<32>(0);
            cout << "WrtData          : " << WrtData << endl;
            // For R-Type Initialisation, WrtEnable is 0
            WrtEnable = 0;
            cout << "WrtEnable        : " << WrtEnable << endl;
            // Updating Registers..
            myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
        }

        else if (InstructionType == 'J')
        {
            // Initialization Logic for J-type Instructions
            RdReg1 = bitset<5>(0);
            // cout << "Read Register 1  : " << RdReg1 << endl;
            RdReg2 = bitset<5>(0);
            // cout << "Read Register 2  : " << RdReg2 << endl;
            WrtReg = bitset<5>(0);
            // cout << "Write Register   : " << WrtReg << endl;
            WrtData = bitset<32>(0);
            // cout << "WrtData          : " << WrtData << endl;
            WrtEnable = 0;
            // cout << "WrtEnable        : " << WrtEnable << endl;

            bitset<32> nextInstruction = incrementBy(programCounter, 4);
            string jumpAddressString = nextInstruction.to_string().substr(0, 4) + InstructionString.substr(6) + "00";
            jumpAddress = bitset<32>(jumpAddressString);
            cout << "Jump Address     : " << jumpAddress << endl;
        }

        else if (InstructionType == 'I' || InstructionType == 'L' || InstructionType == 'S' || InstructionType == 'B')
        {
            // Initialization Logic for I-type Instructions
            RdReg1 = bitset<5>(InstructionString.substr(6, 5));
            cout << "Read Register 1  : " << RdReg1 << endl;
            // In I-type Instructions, RdReg2 is not required, however we require values from ReadData2 in Store
            // instructions, hence we pass WrtReg here...
            RdReg2 = bitset<5>(InstructionString.substr(11, 5));
            // cout << "Read Register 2  : " << RdReg2 << endl;
            WrtReg = bitset<5>(InstructionString.substr(11, 5));
            cout << "Write Register   : " << WrtReg << endl;
            imm = bitset<16>(InstructionString.substr(17));
            cout << "Immediate Value  : " << imm << endl;
            // For I-type Initiliasation, WrtData is 0
            WrtData = bitset<32>(0);
            cout << "Write Data       : " << WrtData << endl;
            // For I-type Initiliasation, WrtEnable is 0
            WrtEnable = 0;
            cout << "WrtEnable        : " << WrtEnable << endl;
            // // Depending on the subtype of the I-type instruction, the WrtEnable bit is set
            // if (InstructionType == 'S' || InstructionType == 'B') {
            //   WrtEnable = 0;
            // } else WrtEnable = 1;
            // Updating Registers...
            myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
        }

        // ----------------------
        //  EXECUTION PHASE
        // ----------------------
        if (InstructionType == 'R')
        {
            // Execution Logic for R-type Instructions
            cout << "" << endl;
            cout << "   - Execution -  " << endl;
            // @todo check the execution logic for AND, OR and NOR instructions (do we need to extend numbers)...
            cout << "Register 1 Data  : " << myRF.ReadData1 << endl;
            cout << "Register 2 Data  : " << myRF.ReadData2 << endl;
            myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);
            cout << "ALU Result       : " << myALU.ALUresult << endl;
        }
        else if (InstructionType == 'B')
        {
            cout << "" << endl;
            cout << "   - Execution -  " << endl;
            // Comparing whether the operands are equal by checking whether the substraction of both operands is equal
            // to 0
            cout << "Register 1 Data  : " << myRF.ReadData1 << endl;
            cout << "Register 2 Data  : " << myRF.ReadData2 << endl;
            myALU.ALUOperation(SUBU, myRF.ReadData1, myRF.ReadData2);
            cout << "ALU Result       : " << myALU.ALUresult << endl;
            if (myALU.ALUresult.to_ulong() == 0)
            {
                isEqual = 1;
            }
            else
                isEqual = 0;
        }
        else if (InstructionType == 'L' || InstructionType == 'S')
        {
            cout << "" << endl;
            cout << "   - Execution -  " << endl;
            ALUOP = bitset<3>("001");
            cout << "ALUOP            : " << ALUOP << endl;
            // Sign extending imm
            bitset<32> signExtendedImm = signExtend(imm);
            cout << "Sign Extended Imm: " << signExtendedImm << endl;
            cout << "Register 1 Data  : " << myRF.ReadData1 << endl;
            cout << "Register 2 Data  : " << myRF.ReadData2 << endl;
            // Calculating the resulting address from ALU
            Address = myALU.ALUOperation(ALUOP, signExtendedImm, myRF.ReadData1);
            cout << "ALU Result       : " << Address << endl;
        }
        else if (InstructionType != 'J')
        {
            cout << "" << endl;
            cout << "   - Execution -  " << endl;
            ALUOP = bitset<3>(InstructionString.substr(3, 3));
            cout << "ALUOP            : " << ALUOP << endl;
            bitset<32> signExtendedImm = signExtend(imm);
            myALU.ALUOperation(ALUOP, myRF.ReadData1, signExtendedImm);
            cout << "ALU Result       : " << Address << endl;
        }

        // ----------------------
        //  READ/WRITE PHASE
        // ----------------------
        if (InstructionType == 'L')
        {
            cout << "" << endl;
            cout << "  - R/W Memory -  " << endl;
            readmem = 1;
            writemem = 0;
            myDataMem.MemoryAccess(Address, bitset<32>(0), readmem, writemem);
            cout << "Data from Memory : " << myDataMem.readdata << endl;
        }
        else if (InstructionType == 'S')
        {
            cout << "" << endl;
            cout << "  - R/W Memory -  " << endl;
            readmem = 0;
            writemem = 1;
            cout << "Data in Register : " << myRF.ReadData2 << endl;
            myDataMem.MemoryAccess(Address, myRF.ReadData2, readmem, writemem);
            cout << "Data Writen      : " << myRF.ReadData2 << endl;
        }
        else
        {
            readmem = 0;
            writemem = 0;
        }

        // ----------------------
        //  UPDATE REGISTERS
        // ----------------------
        if (InstructionType == 'L')
        {
            cout << "" << endl;
            cout << " - R/W Register - " << endl;
            WrtData = myDataMem.readdata;
            cout << "Write Data       : " << WrtData << endl;
            WrtEnable = 1;
            cout << "WrtEnable        : " << WrtEnable << endl;
            myRF.ReadWrite(RdReg1, WrtReg, WrtReg, WrtData, WrtEnable);
        }
        else if (InstructionType == 'R')
        {
            cout << "" << endl;
            cout << " - R/W Register - " << endl;
            WrtData = myALU.ALUresult;
            cout << "Write Data       : " << WrtData << endl;
            WrtEnable = 1;
            cout << "WrtEnable        : " << WrtEnable << endl;
            myRF.ReadWrite(RdReg1, RdReg2, WrtReg, WrtData, WrtEnable);
        }
        else if (InstructionType == 'I')
        {
            cout << "" << endl;
            cout << " - R/W Register - " << endl;
            WrtData = myALU.ALUresult;
            cout << "Write Data       : " << WrtData << endl;
            WrtEnable = 1;
            cout << "WrtEnable        : " << WrtEnable << endl;
            // @todo Check if there are no write commands to update the registers
            myRF.ReadWrite(RdReg1, WrtReg, WrtReg, WrtData, WrtEnable);
        }

        // ----------------------
        //  UPDATING PC
        // ----------------------
        if (InstructionType == 'J')
        {
            programCounter = jumpAddress;
        }
        else if (InstructionType == 'B' && isEqual.to_ulong())
        {
            bitset<32> nextInstruction = incrementBy(programCounter, 4);
            bitset<32> signExtendedImm = signExtend(imm);
            string branchAddressString = signExtendedImm.to_string().substr(2) + "00";
            bitset<32> branchAddress(branchAddressString);
            cout << "Branch Address   : " << WrtEnable << endl;
            programCounter = bitset<32>(nextInstruction.to_ulong() + branchAddress.to_ulong());
        }
        else
        {
            programCounter = incrementBy(programCounter, 4);
        }

        cout << "--------------------------------------------------------" << endl;

        /**** You don't need to modify the following lines. ****/
        myRF.OutputRF(); // dump RF;
    }
    myDataMem.OutputDataMem(); // dump data mem

    return 0;
}
