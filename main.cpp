#include <iostream>
#include <fstream>
#include <string>
#include <utility>

class Console {
public:
    enum message_lvl {
        message,
        information,
        warning,
        error
    };
public:
    Console() {}

    void Print(const Console::message_lvl lvl, const std::string message, const int &line_indx) {
        if ( lvl == Console::message_lvl::message ) { std::cout << "[Message]: " << message << '\n';} // white text
        else if ( lvl == Console::message_lvl::information ) { std::cout << "[Information]: " << message << ", line: " << line_indx << '\n'; } // Blue text
        else if ( lvl == Console::message_lvl::warning ) { std::cout << "[Warning]: " << message << ", line: " << line_indx << '\n'; } // Yellow Text
        else if ( lvl == Console::message_lvl::error ) { std::cout << "[Error]: " << message << ", line: " << line_indx << '\n'; } // Red Text
    }
};

class Compiler {
private:
    std::fstream file;
    Console console;

    int line_n;
    int line_indx;
    std::string line;

    enum argument_types {
        variable,
        string,
        error,
        unknown
    };
public:

public:
    Compiler(const std::string filename, Console &console)
    : file(filename), console(console)
    {

    }

    void Start() {
        line_indx = 0;

        std::pair<std::string, int> instruction;
        while ( std::getline(file, line) ) {
            line_n = line.size();

            instruction = Get_Instruction();

            if ( instruction.first == "" ) { console.Print(Console::message_lvl::error, "No instruction found", line_indx); return; }
            
            console.Print(console.information, "Instruction: " + instruction.first, line_indx);

            if      ( instruction.first == "print" )  { Print(instruction.second); }
            else if ( instruction.first == "int" )    {}
            else if ( instruction.first == "string" ) {}
            else if ( instruction.first == "add" )    {}
            else if ( instruction.first == "sub" )    {}
            else if ( instruction.first == "mul" )    {}
            else if ( instruction.first == "div" )    {}
            else {
                console.Print(console.error, "Instruction not recognised ->" + instruction.first, line_indx);
                return;
            }

            line_indx ++;
        }
    }

    std::pair<std::string, int> Get_Instruction() {
        std::pair<std::string, int> ret{"", 0}; // Instruction, pointer

        for (int i = 0; i < line_n; i++) {
            if ( line[i] == ' ' ) { break; }
            ret.first += line[i];
            ret.second ++;
        }

        ret.second++; // Return pointer to the next thing after the space
        return ret;
    }

    void Print(int &poi) {
        console.Print(console.information, "Print detected", line_indx);
        std::pair<argument_types, std::string> argument_inf = Get_Argument(poi);
        if ( argument_inf.first == error ) {return;}

        if ( argument_inf.first == string ) {
            std::cout << argument_inf.second;
        }
    }

    std::pair<argument_types, std::string> Get_Argument(int &poi) {
        if ( poi >= line_n ) { console.Print(console.error, "No argument provided", line_indx); }
        std::string argument = "";

        bool is_string = false; // False = variable, True = string 
        bool string_ended = false;
        if ( line[poi] == '"' ) { is_string = true; }

        poi++;
        argument_types type = unknown;
        for (; poi < line_n; poi++) {
            if ( string_ended  )    { console.Print(console.error, "String ended, wtf is this -> " + line[poi], line_indx); type = error; break;}
            if ( line[poi] == '"' ) { string_ended = true; }
            if ( line[poi] == ' ' ) { break; }
            argument += line[poi];
        }

        if ( argument == "" ) { console.Print(console.error, "No argument provided", line_indx); type = error; }

        if ( type != error) {
            if ( is_string ) { type = string; }
            else { type = variable; }
        }

        console.Print(console.information, "Argument inf: " + argument + ", type: " + (type == error ? "Error" : type == variable ? "Variable" : "String"), line_indx);
        return std::pair<argument_types, std::string>{type, argument};
    }
};

int main() {
    Console cons;
    Compiler comp("text.lang", cons);

    comp.Start();    

    return 0;
}