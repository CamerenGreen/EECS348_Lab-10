#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include <string>
#include <vector>

class Calculator {
public:
    // Parse a string into a double (not used for validation)
    static double parse_number(const std::string& s);
    
    // Validates if a string represents a valid double number
    static bool isValidDouble(const std::string &s);
    
    // Adds two numbers represented as strings
    static std::string addStrings(const std::string &num1, const std::string &num2);
    
    // Processes numbers from a file and performs operations
    static void processNumbersFromFile(const std::string &filename);
    
private:
    // Helper function to align two numbers for addition/subtraction
    static void alignNumbers(std::string &num1, std::string &num2);
    
    // Helper function to perform actual string addition
    static std::string performAddition(const std::string &num1, const std::string &num2);
    
    // Helper function to perform actual string subtraction
    static std::string performSubtraction(std::string num1, std::string num2);
    
    // Helper to clean up result (remove trailing zeros, etc.)
    static std::string cleanResult(std::string result, size_t dotPos);
};

#endif // CALCULATOR_HPP
