#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>
#include "help.cpp"

/* Global variables */
// variables for creating variables ???
enum variable_types {
    int_,
    float_,
    string_,
    error
};

std::unordered_map<std::string, std::pair<variable_types, int>> variable_names; // variable_name, index of variable
std::unordered_map<int, int> int_values; // Index of variable, value
std::unordered_map<int, float> float_values;
std::unordered_map<int, std::string> string_values;



/* Console for printing stuff */
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



/* Language Compiler */
class Compiler {
private:
    std::fstream file; 
    Console console; // Console for printing stuff

    int line_n; // length of each line
    int line_indx; // index of each line
    std::string line; // each line stored for processing

    int variable_indx = 0;
    std::string loop_variable_name; // some loop variable
    int loop_variable_indx = 0;
    std::vector<std::string> loop; // each line in the loop is stored here
    
    // Types of what each argument can be
    // "hehe" -> string
    // i -> variable
    // 123 -> number
    // 6i" -> error
    enum argument_types { 
        variable,
        string,
        number,
        error,
        unknown
    };

    std::string digits = "1234567890.";
public:

public:
    Compiler(const std::string filename, Console &console)
    : file(filename), console(console)
    {

    }

    // Main function for reading each line of the language
    void Main() {
        line_indx = 0;
        while (std::getline(file, line)) {
            Each_Line_Process(line);
            line_indx++;
        }
    }
private:
    // Processes each line
    // Reads instruction in each line and proceeds to instruction's function
    void Each_Line_Process(const std::string line) {
        std::pair<std::string, int> instruction;
        line_n = line.size();

        instruction = Get_Instruction();

        if ( instruction.first == "" ) { console.Print(Console::message_lvl::error, "No instruction found", line_indx); return; }
        
        console.Print(console.information, "Instruction: " + instruction.first, line_indx);

        if      ( instruction.first == "print" )  { Print(instruction.second); }
        else if ( instruction.first == "int" )    { Int(instruction.second); }
        else if ( instruction.first == "string" ) { String(instruction.second); }
        else if ( instruction.first == "add" )    { Add(instruction.second); }
        else if ( instruction.first == "sub" )    { Sub(instruction.second); }
        else if ( instruction.first == "mul" )    { Mul(instruction.second); }
        else if ( instruction.first == "div" )    { }
        else if ( instruction.first == "for")     { For(instruction.second); }
        else if ( instruction.first == "end")     {}
        else {
            console.Print(console.error, "Instruction not recognised ->" + instruction.first, line_indx);
            return;
        }

    }

    // Reads and checks line's instruction
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

    // Checks next argument and decides what type of argument it is ( check argument_types )
    std::pair<argument_types, std::string> Get_Argument(int &poi) {
        if ( poi >= line_n ) { console.Print(console.error, "No argument provided", line_indx); }
        std::string argument = "";

        short type_prediction = 0; // 0 = variable, 1 = string, 2 = number
        bool string_ended = false;
        //std::cout << "Line[poi]: " << line[poi] << ' ' << poi << '\n';
        if ( line[poi] == '"' ) { type_prediction = 1; }
        else if ( digits.find(line[poi]) != std::string::npos ) { type_prediction = 2; }
        //std::cout << "Prediction: " << type_prediction << '\n';

        //poi++;
        argument_types type = unknown;
        for (; poi < line_n; poi++) {
            //std::cout << poi << '\n';
            //if ( string_ended  )    { console.Print(console.error, "String ended, wtf is this -> " + line[poi], line_indx); type = error; break;}
            //if ( line[poi] == '"' ) { string_ended = true; }
            if ( line[poi] == ' ' ) { break; }
            argument += line[poi];
        }
        poi++;

        if ( argument == "" ) { console.Print(console.error, "No argument provided", line_indx); type = error; }

        if ( type != error) {
            if ( type_prediction == 1 ) { type = string; }
            else if (type_prediction == 0) { type = variable; }
            else if (type_prediction == 2) { type = number; }
            else {
                console.Print(console.error, "Can't decide argument type, in Get_Argument", line_indx);
                type = error;
            }
        }

        console.Print(console.information, "Argument inf: " + argument + ", type: " + (type == error ? "Error" : type == variable ? "Variable" : type == string ? "String" : "Number"), line_indx);
        return std::pair<argument_types, std::string>{type, argument};
    }

    // "print" instruction ( prints somethign to the screen... )
    int Print(int &poi) {
        std::cout << "Printing line: " << line << '\n';
        //console.Print(console.information, "Print detected", line_indx);
        std::pair<argument_types, std::string> argument_inf = Get_Argument(poi);
        if ( argument_inf.first == error ) {return 1;}

        std::string variable_type;
        if ( argument_inf.first == string ) {
            std::cout << argument_inf.second;
        }
        else if ( argument_inf.first == variable ) {
            if ( !dic_find(variable_names, argument_inf.second) ) { console.Print(console.error, "Variable doens't exist -> " + argument_inf.second, line_indx); return 1; }

            if ( variable_names[argument_inf.second].second == variable_types::int_ ) {
                std::cout << "Variable: " << argument_inf.second <<", value: " << int_values[variable_names[argument_inf.second].second] <<", type: INT\n";
            }
            if ( variable_names[argument_inf.second].second == variable_types::float_ ) {
                std::cout << "Variable: " << argument_inf.second <<", value: " << string_values[variable_names[argument_inf.second].second] <<", type: STRING\n";
            }
            if ( variable_names[argument_inf.second].second == variable_types::string_ ) { 
                std::cout << "Variable: " << argument_inf.second <<", value: " << string_values[variable_names[argument_inf.second].second] <<", type: FLOAT\n";
            }
        }
        return 0;
    }

    // Declares int variable
    int Int(int &poi) {
        //std::cout << poi << ' ' << line_n << '\n';
        std::pair<argument_types, std::string> argument_inf = Get_Argument(poi);
        //std::cout << argument_inf.second << '\n';

        if ( argument_inf.first != argument_types::variable ) { console.Print(console.error, "Argument isn't valid -> " + argument_inf.second, line_indx); return 1; }

        if ( dic_find(variable_names, argument_inf.second) ) { console.Print(console.error, "Variable already exists ->" + argument_inf.second, line_indx); return 1; }

        variable_names[argument_inf.second] = std::pair<variable_types, int>{variable_types::int_, variable_indx};
        
        std::pair<argument_types, std::string> value_inf = Get_Argument(poi);
        //std::cout << argument_inf.second << ' ' << value_inf.second << '\n';
        //std::cout << value_inf.second << '\n';
        std::string type;
        if ( value_inf.first != argument_types::number && value_inf.first != argument_types::variable) {
            console.Print(console.error, "Value must be number or variable -> " + value_inf.second, line_indx);
            return 1;
        }
        else if ( value_inf.first == argument_types::number ) {
            int_values[variable_indx] = std::stoi(value_inf.second);
            console.Print(console.information, "Variable Created, type: INT value: " + value_inf.second, line_indx);
            variable_indx++;
        }
        else if ( value_inf.first == argument_types::variable ) {
            std::pair<variable_types, int> adding_var_inf = variable_names[value_inf.second];
            
            if ( adding_var_inf.first == variable_types::int_ ) {
                int_values[variable_indx] = int_values[adding_var_inf.second];
                console.Print(console.information, "Variable Created, type: INT value: " + int_values[adding_var_inf.second], line_indx);
                variable_indx++;
            }
            else if ( adding_var_inf.first == variable_types::float_ ) {
                int_values[variable_indx] = float_values[adding_var_inf.second];
                int val = (int) float_values[adding_var_inf.second];
                console.Print(console.information, "Variable Created, type: FLOAT value: " + val, line_indx);
                variable_indx++;
            }
            else {
                console.Print(console.error, "Variable type must be numeric -> " + value_inf.second, line_indx);
                return 1;
            }
            
        }

        return 0;
    }

    // Declares string variable
    int String(int &poi) {
        std::pair<argument_types, std::string> argument1_inf = Get_Argument(poi);
        std::pair<argument_types, std::string> argument2_inf = Get_Argument(poi);

        if ( argument2_inf.first != argument_types::variable ) { console.Print(console.error, "Something idk -> " + argument1_inf.second, line_indx); }
        if ( argument2_inf.first != argument_types::string ) { console.Print(console.error, "2nd argument must be a string -> " + argument2_inf.second, line_indx); }
        if ( dic_find(variable_names, argument1_inf.second) ) { console.Print(console.error, "Variable already exists -> " + argument1_inf.second, line_indx); }

        variable_names[argument1_inf.second] = {variable_types::string_, variable_indx};
        string_values[variable_indx] = argument2_inf.second;
        variable_indx++;
    }

    // 
    std::string String_Type(variable_types type) {
        if ( type == variable_types::int_ ) { return "INT"; }
        else if ( type == variable_types::float_  ) { return "FLOAT"; }
        else if ( type == variable_types::string_ ) { return "STRING"; }
        else { return "ERROR"; }
    }

    // Adds somethign to the variable
    int Add(int &poi) {
        std::pair<argument_types, std::string> argument1_inf = Get_Argument(poi);
        std::pair<argument_types, std::string> argument2_inf = Get_Argument(poi);
        if ( argument1_inf.first != argument_types::variable ) { console.Print(console.error, "1st argument must be a variable!", line_indx); return 1; }
        
        std::pair<variable_types, int> argument1_val = variable_names[argument1_inf.second];

        if ( argument2_inf.first == argument_types::variable ) {
            std::pair<variable_types, int> argument2_val = variable_names[argument2_inf.second];

            if ( argument1_val.first == variable_types::int_ && argument2_val.first == variable_types::int_ ) {
                int_values[argument1_val.second] += int_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::int_ && argument2_val.first == variable_types::float_ ) {
                int_values[argument1_val.second] += float_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::float_ && argument2_val.first == variable_types::int_ ) {
                float_values[argument1_val.second] += int_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::float_ && argument2_val.first == variable_types::float_ ) {
                float_values[argument1_val.second] += float_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::string_ && argument2_val.first == variable_types::string_ ) {

            }
            else {
                console.Print(console.error, "Variable types are incorrect / not implemented :3", line_indx);
                return 1;
            }
        }
        else if ( argument2_inf.first == argument_types::number ) {
            if ( argument1_val.first == variable_types::int_ ) {
                int_values[argument1_val.second] += std::stoi(argument2_inf.second);
            }
            else if ( argument1_val.first == variable_types::float_ ) {
                float_values[argument1_val.second] += std::stoi(argument2_inf.second);
            }
            else {
                console.Print(console.error, "U can't add number to -> " + String_Type(argument1_val.first), line_indx );
                return 1;
            }
        }
        else if ( argument2_inf.first == argument_types::string ) {
            if ( argument1_val.first == variable_types::string_ ) {
                string_values[argument1_val.second] += argument2_inf.second;
            }
            else {
                console.Print(console.error, "U can only add string to string variables !", line_indx);
                return 1;
            }
        }

        return 0;
    }

    // Substracts something from the variable
    int Sub(int &poi) {
        std::pair<argument_types, std::string> argument1_inf = Get_Argument(poi);
        std::pair<argument_types, std::string> argument2_inf = Get_Argument(poi);
        if ( argument1_inf.first != argument_types::variable ) { console.Print(console.error, "1st argument must be a variable!", line_indx); return 1; }
        
        std::pair<variable_types, int> argument1_val = variable_names[argument1_inf.second];

        if ( argument2_inf.first == argument_types::variable ) {
            std::pair<variable_types, int> argument2_val = variable_names[argument2_inf.second];

            if ( argument1_val.first == variable_types::int_ && argument2_val.first == variable_types::int_ ) {
                int_values[argument1_val.second] -= int_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::int_ && argument2_val.first == variable_types::float_ ) {
                int_values[argument1_val.second] -= float_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::float_ && argument2_val.first == variable_types::int_ ) {
                float_values[argument1_val.second] -= int_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::float_ && argument2_val.first == variable_types::float_ ) {
                float_values[argument1_val.second] -= float_values[argument2_val.second];
            }
            else {
                console.Print(console.error, "Variable types are incorrect / not implemented :3", line_indx);
                return 1;
            }
        }
        else if ( argument2_inf.first == argument_types::number ) {
            if ( argument1_val.first == variable_types::int_ ) {
                int_values[argument1_val.second] -= std::stoi(argument2_inf.second);
            }
            else if ( argument1_val.first == variable_types::float_ ) {
                float_values[argument1_val.second] -= std::stoi(argument2_inf.second);
            }
            else {
                console.Print(console.error, "U can't substract number to -> " + String_Type(argument1_val.first), line_indx );
                return 1;
            }
        }
        else {
            console.Print(console.error, "U can only substract number or variables", line_indx);
            return 1;
        }

        return 0;
    }

    // Multipiles variable by something
    int Mul(int &poi) {
        std::pair<argument_types, std::string> argument1_inf = Get_Argument(poi);
        std::pair<argument_types, std::string> argument2_inf = Get_Argument(poi);

        if ( argument1_inf.first != argument_types::variable ) { console.Print(console.error, "1st argument must be variable -> " + argument1_inf.second, line_indx); }
        
        std::pair<variable_types, int> argument1_val = variable_names[argument2_inf.second];

        if ( argument2_inf.first == argument_types::variable ) {
            std::pair<variable_types, int> argument2_val = variable_names[argument2_inf.second];

            if ( argument1_val.first == variable_types::int_ && argument2_val.first == variable_types::int_ ) {
                int_values[argument1_val.second] *= int_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::int_ && argument2_val.first == variable_types::float_ ) {
                int_values[argument1_val.second] *= float_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::float_ && argument2_val.first == variable_types::int_ ) {
                float_values[argument1_val.second] *= int_values[argument2_val.second];
            }
            else if ( argument1_val.first == variable_types::float_ && argument2_val.first == variable_types::float_ ) {
                float_values[argument1_val.second] *= float_values[argument2_val.second];
            }
            else {
                console.Print(console.error, "Argument types not compatiable / implemented :3", line_indx);
                return 1;
            }
        }
        else if ( argument2_inf.first == argument_types::number ) {
            if ( argument1_val.first == variable_types::int_ ) {
                int_values[argument1_val.second] *= std::stoi(argument2_inf.second);
            }
            else if ( argument1_val.first == variable_types::float_ ) {
                float_values[argument1_val.second] *= std::stoi(argument2_inf.second);
            }
            else if ( argument1_val.first == variable_types::string_ ) {
                int x = std::stoi(argument2_inf.second);
                std::string temp = string_values[argument1_val.second];
                for (int j = 0; j < x; j++) {
                    string_values[argument1_val.second] += temp;
                }
            }
        }
    }

    // For loop:
    // for variable_name start end step
    int For(int &poi) {
        std::pair<argument_types, std::string> variable_inf = Get_Argument(poi); // variable_name
        std::pair<argument_types, std::string> start_inf = Get_Argument(poi); // start
        std::pair<argument_types, std::string> end_inf = Get_Argument(poi); // end
        std::pair<argument_types, std::string> step_inf = Get_Argument(poi); // step

        if ( variable_inf.first != argument_types::variable ) { console.Print(console.error, "1st argumnet must be variable -> " + variable_inf.second, line_indx); return 1;}
        if ( start_inf.first != argument_types::number ) { console.Print(console.error, "2st argumnet (start) must be number -> " + start_inf.second, line_indx); return 1;}
        if ( end_inf.first != argument_types::number ) { console.Print(console.error, "3st argumnet (end) must be variable -> " + end_inf.second, line_indx); return 1;}
        if ( step_inf.first != argument_types::number ) { console.Print(console.error, "4st argumnet (step) must be variable -> " + step_inf.second, line_indx); return 1; }

        while ( std::getline(file, line) && line != "end" ) {
            std::cout << "Line: " << line << '\n';
            loop.push_back(line);
            line_indx++;
        }
        std::cout << "Whole loop: \n";
        for (std::string i : loop) {
            std::cout << i << '\n';
        }
        std::cout << "end\n";

        int start = std::stoi(start_inf.second);
        int end = std::stoi(end_inf.second);
        int step = std::stoi(step_inf.second);

        loop_variable_name = variable_inf.second;
        variable_names[loop_variable_name] = {variable_types::int_, variable_indx};
        loop_variable_indx = variable_indx;
        variable_indx++;
        int_values[loop_variable_indx] = start;

        for (int i = start; i < end; i+=step) {
            int_values[loop_variable_indx] = i;
            for (std::string lin : loop) {
                line = lin;
                std::cout << "Line: " << lin << '\n';
                Each_Line_Process(lin);
            }
        }
        variable_names.erase(loop_variable_name);
        int_values.erase(loop_variable_indx);
    }
};

int main() {
    Console cons;
    Compiler comp("text.lang", cons);

    comp.Main();    

    return 0;
}