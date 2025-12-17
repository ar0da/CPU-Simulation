#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
bool is_number(const string &s)
{
    if (s.empty())
        return false; // Not a number if empty.

    size_t start = 0;
    // Check for a negative sign at the beginning.
    if (s[0] == '-')
    {
        start = 1;
    }

    if (start >= s.length())
        return false; // Not a number if it's just a "-".

    // Check if all other characters are digits.
    for (size_t i = start; i < s.length(); ++i)
    {
        if (!isdigit(s[i]))
            return false;
    }
    return true;
}
bool is_valid_operan(const string &operand)
{

    // Check for register format, e.g., "R1".
    if (operand.length() == 2 && operand[0] == 'R' && operand[1] >= '1' && operand[1] <= '6')
        return true;
    // Check if it's a number.
    if (is_number(operand) || (operand.length() > 1 && operand[0] == '-' && is_number(operand.substr(1))))
        return true;
    // check for address format, e.g., "#100"
    if (operand.length() >= 2 && operand[0] == '#' && is_number(operand.substr(1)))
        return true;
    return false;
}
class memory
{
private:
    unsigned char mem[100];

public:
    memory()
    {
        for (int i = 0; i < 100; i++)
        {
            mem[i] = 0;
        }
    }
    unsigned char get(int address)
    {
        if (address < 0 || address >= 100)
        {
            cerr << "Error: Invalid memory address access" << endl;
            exit(1);
        }
        return mem[address];
    }
    void set(int address, unsigned char value)
    {
        if (address < 0 || address >= 100)
        {
            cerr << "Error: Invalid memory address access" << endl;
            exit(1);
        }
        mem[address] = value;
    }
    void print()
    {
        cout << "Memory State:" << endl;
        for (int i = 0; i < 100; ++i)
        {
            cout << "#" << i << ": " << int(mem[i]) << " ";
            if ((i + 1) % 10 == 0)
            { // Print 10 memory addresses per line.
                cout << endl;
            }
        }
        cout << endl;
    }
};
class cpu
{
private:
    int reg[6];
    int program_counter;
    memory mem;
    vector<string> instructions;
    bool halted = false;

public:
    cpu() : program_counter(0)
    {
        for (int i = 0; i < 6; i++)
        {
            reg[i] = 0;
        }
    }
    void process_instruction_file(const string &filename)
    {
        ifstream file(filename); // Open the file.
        if (!file.is_open())
        {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }
        string line;
        string instruction;
        // Read the file line by line.
        while (getline(file, line))
        {
            stringstream line_stream(line);
            // Get the part of the line before any ';', which is the comment character.
            getline(line_stream, instruction, ';');
            instructions.push_back(instruction); // Add the clean instruction to the vector.
        }
        file.close();
        if (instructions.size() > 500)
        {
            cerr << "Error: Too many instructions. Maximum allowed is 500." << endl;
            exit(1);
        }
    }
    void run(int option)
    {
        while (program_counter < instructions.size() && !halted)
        {
            string currentInstruction = instructions[program_counter];
            if (option == 1 || option == 2)
            {
                cout << currentInstruction << " - ";
                printRegisters();
            }
            if (option == 2)
            {
                mem.print();
            }
            execute(currentInstruction);
        }
        cout << "Final register states:" << endl;
        printRegisters();
        cout << "Final memory state:" << endl;
        mem.print();
    }

private:
    void execute(const string &instruction)
    {
        stringstream ss;
        ss.str(instruction);
        string opcode, operand1, operand2;
        ss >> opcode; // Read the instruction (e.g., MOV, ADD).
        if (opcode == "HLT")
        {
            halted = true;
            cout << "Program halted at line " << program_counter + 1 << endl;
            return;
        }
        ss >> operand1; // Read the first operand.
        ss >> operand2; // Read the second operand (if it exists).

        if (!operand1.empty() && operand1.back() == ',')
        {
            operand1.pop_back();
        }
        if (is_valid_operan(operand1) == false)
        {
            cerr << "Error: Invalid operand '" << operand1 << "' at line " << program_counter + 1 << endl;
            exit(1);
        }
        if (operand2.empty())
        {
            operand2 = "none";
        }
        else if (is_valid_operan(operand2) == false)
        {
            cerr << "Error: Invalid operand '" << operand2 << "' at line " << program_counter + 1 << endl;
            exit(1);
        }
        int next_pc = program_counter + 1;

        if (opcode == "MOV")
            mov(operand1, operand2);
        else if (opcode == "ADD")
            add(operand1, operand2);
        else if (opcode == "SUB")
            sub(operand1, operand2);
        else if (opcode == "PRN")
            prn(operand1);
        else if (opcode == "INP")
            inp(operand1);
        else if (opcode == "JMP")
            jmp(operand1, operand2);
        else
        {
            cerr << "Error: Invalid instruction '" << opcode << "' at line " << program_counter + 1 << endl;
            exit(1);
        }
        if (program_counter == next_pc - 1)
        {
            program_counter = next_pc;
        }
    }
    void mov(const string &operand1, const string &operand2)
    {
        // Case 1: MOV reg, reg (e.g., MOV R1, R2). This copies R1's value into R2.
        if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == 'R' && isdigit(operand2[1]))
        {
            int reg_index1 = operand1[1] - '1';
            int reg_index2 = operand2[1] - '1';
            if (reg_index1 < 0 || reg_index1 >= 6 || reg_index2 < 0 || reg_index2 >= 6)
            {
                cerr << "Error: Invalid register in MOV instruction" << endl;
                exit(1);
            }
            reg[reg_index2] = reg[reg_index1]; // R2 = R1
        }
        // case 2: MOV reg, constant (e.g., MOV R1, 100)
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && is_number(operand2))
        {
            int reg_index = operand1[1] - '1';
            int value = stoi(operand2);
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in MOV instruction" << endl;
                exit(1);
            }
            reg[reg_index] = value; // R1 = 100
        }
        // case 3: MOV reg, adress (e.g., MOV R1, #100)copies the value of R2 to memory address 100.
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == '#' && is_number(operand2.substr(1)))
        {
            int reg_index = operand1[1] - '1';
            int address = stoi(operand2.substr(1));
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in MOV instruction" << endl;
                exit(1);
            }
            if (address < 0 || address >= 100)
            {
                cerr << "Error: Invalid memory address in MOV instruction" << endl;
                exit(1);
            }
            mem.set(address, reg[reg_index]); // memory[address] = R1
        }
        // case 4: MOV adress, reg (e.g., MOV #100, R1)
        else if (operand1[0] == '#' && is_number(operand1.substr(1)) && operand2[0] == 'R' && isdigit(operand2[1]))
        {
            int address = stoi(operand1.substr(1));
            int reg_index = operand2[1] - '1';
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in MOV instruction" << endl;
                exit(1);
            }
            if (address < 0 || address >= 100)
            {
                cerr << "Error: Invalid memory address in MOV instruction" << endl;
                exit(1);
            }
            reg[reg_index] = mem.get(address); // R1 = memory[address]
        }
        // case 5: MOV adress, constant (e.g., MOV #100, 10)
        else if (operand1[0] == '#' && is_number(operand1.substr(1)) && is_number(operand2))
        {
            int address = stoi(operand1.substr(1));
            int value = stoi(operand2);
            if (address < 0 || address >= 100)
            {
                cerr << "Error: Invalid memory address in MOV instruction" << endl;
                exit(1);
            }
            mem.set(address, value); // memory[address] = constant
        }
        else
        {
            cerr << "Error: Invalid operands for MOV instruction" << endl;
            exit(1);
        }
    }
    void sub(const string &operand1, const string &operand2)
    {
        // case 1: SUB reg, reg (e.g., SUB R1, R2) // R1 = R1 - R2
        if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == 'R' && isdigit(operand2[1]))
        {
            int reg_index1 = operand1[1] - '1';
            int reg_index2 = operand2[1] - '1';
            if (reg_index1 < 0 || reg_index1 >= 6 || reg_index2 < 0 || reg_index2 >= 6)
            {
                cerr << "Error: Invalid register in SUB instruction" << endl;
                exit(1);
            }
            reg[reg_index1] -= reg[reg_index2]; // R1 = R1 - R2
        }
        //  Case 2: SUB reg, constant (e.g., SUB R1, 1) // R1 = R1 - 1
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && is_number(operand2))
        {
            int reg_index = operand1[1] - '1';
            int value = stoi(operand2);
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in SUB instruction" << endl;
                exit(1);
            }
            reg[reg_index] -= value; // R1 = R1 - 1
        }
        //  case 3: SUB reg, adress (e.g., SUB R1, #100) // memory[address]=memory[address]-R1
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == '#' && is_number(operand2.substr(1)))
        {
            int reg_index = operand1[1] - '1';
            int address = stoi(operand2.substr(1));
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in SUB instruction" << endl;
                exit(1);
            }
            if (address < 0 || address >= 100)
            {
                cerr << "Error: Invalid memory address in SUB instruction" << endl;
                exit(1);
            }
            reg[reg_index] -= mem.get(address); // R1 = R1 - memory[address]
        }
        else
        {
            cerr << "Error: Invalid operands for SUB instruction" << endl;
            exit(1);
        }
    }
    void add(const string &operand1, const string &operand2)
    {
        // case 1: ADD reg, reg (e.g., ADD R1, R2) // R1 = R1 + R2
        if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == 'R' && isdigit(operand2[1]))
        {
            int reg_index1 = operand1[1] - '1';
            int reg_index2 = operand2[1] - '1';
            if (reg_index1 < 0 || reg_index1 >= 6 || reg_index2 < 0 || reg_index2 >= 6)
            {
                cerr << "Error: Invalid register in ADD instruction" << endl;
                exit(1);
            }
            reg[reg_index1] += reg[reg_index2]; // R1 = R1 + R2
        }
        //  Case 2: ADD reg, constant (e.g., ADD R1, 1) // R1 = R1 + 1
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && is_number(operand2))
        {
            int reg_index = operand1[1] - '1';
            int value = stoi(operand2);
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in ADD instruction" << endl;
                exit(1);
            }
            reg[reg_index] += value; // R1 = R1 + 1
        }
        //  case 3: ADD reg, adress (e.g., ADD R1, #100) // memory[address]=memory[address]+R1
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == '#' && is_number(operand2.substr(1)))
        {
            int reg_index = operand1[1] - '1';
            int address = stoi(operand2.substr(1));
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in ADD instruction" << endl;
                exit(1);
            }
            if (address < 0 || address >= 100)
            {
                cerr << "Error: Invalid memory address in ADD instruction" << endl;
                exit(1);
            }
            reg[reg_index] += mem.get(address); // R1 = R1 + memory[address]
        }
        else
        {
            cerr << "Error: Invalid operands for ADD instruction" << endl;
            exit(1);
        }
    }
    void prn(const string &operand1)
    {
        // Case 1: PRN reg (e.g., PRN R1)
        if (operand1[0] == 'R' && isdigit(operand1[1]))
        {
            int reg_index = operand1[1] - '1';
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in PRN instruction" << endl;
                exit(1);
            }
            cout << reg[reg_index] << endl; // Print the value of the register.
        }
        // Case 2: PRN constant (e.g., PRN 10)
        else if (is_number(operand1) || (operand1[0] == '-' && isdigit(operand1[1])))
        {
            int value = stoi(operand1);
            cout << value << endl; // Print the constant value.
        }
        // case 3 PRN adress (e.g., PRN #100)
        else if (operand1[0] == '#' && is_number(operand1.substr(1)))
        {
            int address = stoi(operand1.substr(1));
            if (address < 0 || address >= 100)
            {
                cerr << "Error: Invalid memory address in PRN instruction" << endl;
                exit(1);
            }
            cout << int(mem.get(address)) << endl; // Print the value at the memory address.
        }
        else
        {
            cerr << "Error: Invalid operand for PRN instruction" << endl;
            exit(1);
        }
    }
    void inp(const string &operand1)
    {

        // Case 1: INP reg (e.g., INP R1)
        if (operand1[0] == 'R' && isdigit(operand1[1]))
        {
            int reg_index = operand1[1] - '1';
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register in INP instruction" << endl;
                exit(1);
            }
            int value;
            cout << "Enter an integer value for " << operand1 << ": ";
            cin >> value;
            reg[reg_index] = value; // Store the input value in the register.
        }
        else
        {
            cerr << "Error: Invalid operand for INP instruction" << endl;
            exit(1);
        }
    }
    void jmp(const string &operand1, const string &operand2)
    {
        // Case 1: JMP lineAddress (e.g., JMP 10)
        if (operand2 == "none" && is_number(operand1))
        {
            int line_number = stoi(operand1);
            // Check if the line number is valid.
            if (line_number < 1 || line_number > instructions.size())
            {
                cerr << "Error: Jump to invalid line number " << line_number << endl;
                exit(1);
            }
            program_counter = line_number - 1; // Set program counter to the new line.
        }
        // Case 2: JMP reg, lineAddress (e.g., JMP R1, 10)
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && is_number(operand2))
        {
            int reg_index = operand1[1] - '1'; // Convert register char '1' to index 0.
            int line_number = stoi(operand2);
            // Check if the register is valid.
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register " << operand1 << endl;
                exit(1);
            }
            // Check if the line number is valid.
            if (line_number < 1 || line_number > instructions.size())
            {
                cerr << "Error: Jump to invalid line number " << line_number << endl;
                exit(1);
            }
            // Jump only if the register's value is 0.
            if (reg[reg_index] == 0)
            {
                program_counter = line_number - 1;
            }
        }
        // Case 3: JMP reg, Address (e.g., JMP R1, #100)
        else if (operand1[0] == 'R' && isdigit(operand1[1]) && operand2[0] == '#' && is_number(operand2.substr(1)))
        {
            int reg_index = operand1[1] - '1'; // Convert register char '1' to index 0.
            int address = stoi(operand2.substr(1));
            // Check if the register is valid.
            if (reg_index < 0 || reg_index >= 6)
            {
                cerr << "Error: Invalid register " << operand1 << endl;
                exit(1);
            }
            // Check if the address is valid.
            if (address < 1 || address > instructions.size()) // <--- DÜZELTİLMİŞ HALİ
            {
                // Hata mesajını da düzeltmek iyi olur:
                cerr << "Error: Jump to invalid line number " << address << endl;
                exit(1);
            }
            // Jump only if the register's value is less than or equal to 0.
            if (reg[reg_index] <= 0)
            {
                program_counter = address - 1;
            }
        }
        else
        {
            cerr << "Error: Invalid operands for JMP instruction" << endl;
            exit(1);
        }
    }
    void printRegisters()
    {
        cout << "Register States:" << endl;
        for (int i = 0; i < 6; ++i)
        {
            cout << "R" << (i + 1) << ": " << reg[i] << " ";
        }
        cout << endl;
    }
};
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <filename> <option>" << endl;
        exit(1);
    }

    string filename = argv[1];
    string optionStr = argv[2];

    if (!is_number(optionStr))
    {
        cerr << "Error: Option level must be a number (0, 1 or 2)." << endl;
        exit(1);
    }

    int option = stoi(optionStr);
    if (option < 0 || option > 2)
    {
        cerr << "Error: Invalid option level. Must be 0, 1 or 2." << endl;
        exit(1);
    }

    // 1. CPU Nesnesini Oluştur
    cpu myCPU;

    // 2. Komutları Yükle
    myCPU.process_instruction_file(filename);

    // 3. Çalıştır
    myCPU.run(option);

    return 0;
}
