#include "calculator.hpp"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

double Calculator::parse_number(const std::string& s) {
    std::istringstream iss(s);
    double num;
    iss >> num;
    
    if (iss.fail() || !iss.eof()) {
        throw std::invalid_argument("Invalid double format");
    }
    
    return num;
}

bool Calculator::isValidDouble(const std::string &s) {
    if (s.empty()) return false;
    
    size_t i = 0;
    bool hasDigit = false;
    bool hasDot = false;
    
    // Check for optional sign
    if (s[i] == '+' || s[i] == '-') {
        i++;
    }
    
    // Check digits before decimal point
    while (i < s.size() && isdigit(s[i])) {
        hasDigit = true;
        i++;
    }
    
    // Check for decimal point
    if (i < s.size() && s[i] == '.') {
        hasDot = true;
        i++;
        
        // Must have at least one digit after decimal
        if (i >= s.size() || !isdigit(s[i])) {
            return false;
        }
        
        while (i < s.size() && isdigit(s[i])) {
            i++;
        }
    }
    
    // If we didn't find any digits or didn't consume entire string
    if (!hasDigit || i < s.size()) {
        return false;
    }
    
    return true;
}
void Calculator::alignNumbers(std::string &num1, std::string &num2) {
    size_t dot1 = num1.find('.');
    size_t dot2 = num2.find('.');
    
    // Handle cases where one number has decimal point and the other doesn't
    if (dot1 == std::string::npos && dot2 == std::string::npos) {
        return; // both are integers
    }
    
    if (dot1 == std::string::npos) {
        num1 += ".0";
        dot1 = num1.size() - 2;
    }
    if (dot2 == std::string::npos) {
        num2 += ".0";
        dot2 = num2.size() - 2;
    }
    
    // Align integer parts
    int intLen1 = dot1;
    int intLen2 = dot2;
    
    if (intLen1 < intLen2) {
        num1.insert(0, intLen2 - intLen1, '0');
    } else if (intLen2 < intLen1) {
        num2.insert(0, intLen1 - intLen2, '0');
    }
    
    // Align fractional parts
    int fracLen1 = num1.size() - dot1 - 1;
    int fracLen2 = num2.size() - dot2 - 1;
    
    if (fracLen1 < fracLen2) {
        num1.append(fracLen2 - fracLen1, '0');
    } else if (fracLen2 < fracLen1) {
        num2.append(fracLen1 - fracLen2, '0');
    }
}

std::string Calculator::performAddition(const std::string &num1, const std::string &num2) {
    std::string result;
    int carry = 0;
    
    for (int i = num1.size() - 1; i >= 0; i--) {
        if (num1[i] == '.') {
            result.push_back('.');
            continue;
        }
        int sum = (num1[i] - '0') + (num2[i] - '0') + carry;
        carry = sum / 10;
        result.push_back(sum % 10 + '0');
    }
    
    if (carry) {
        result.push_back(carry + '0');
    }
    
    std::reverse(result.begin(), result.end());
    return result;
}

std::string Calculator::performSubtraction(std::string num1, std::string num2) {
    std::string result;
    int borrow = 0;
    
    for (int i = num1.size() - 1; i >= 0; i--) {
        if (num1[i] == '.') {
            result.push_back('.');
            continue;
        }
        int diff = (num1[i] - '0') - (num2[i] - '0') - borrow;
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.push_back(diff + '0');
    }
    
    std::reverse(result.begin(), result.end());
    return result;
}

std::string Calculator::cleanResult(std::string result, size_t dotPos) {
    // Remove trailing zeros after decimal
    if (dotPos != std::string::npos) {
        while (result.back() == '0' && result.size() > dotPos + 1) {
            result.pop_back();
        }
        // Remove decimal point if no fractional part left
        if (result.back() == '.') {
            result.pop_back();
        }
    }
    
    // Remove leading zeros
    size_t nonZero = result.find_first_not_of("0");
    if (nonZero != std::string::npos && nonZero > 0) {
        if (result[nonZero] == '.') {
            nonZero--; // keep one zero before decimal
        }
        result.erase(0, nonZero);
    }
    
    return result;
}

std::string Calculator::addStrings(std::string num1, std::string num2) {
    // Handle signs
    bool neg1 = !num1.empty() && num1[0] == '-';
    bool neg2 = !num2.empty() && num2[0] == '-';
    
    if (neg1) num1 = num1.substr(1);
    if (neg2) num2 = num2.substr(1);
    
    // Align numbers
    alignNumbers(num1, num2);
    
    // Remove decimal points for easier calculation
    size_t dotPos = num1.find('.');
    if (dotPos != std::string::npos) {
        num1.erase(dotPos, 1);
        num2.erase(dotPos, 1);
    }
    
    std::string result;
    bool resultNegative = false;
    
    // Determine operation
    if (neg1 == neg2) {
        // Both positive or both negative - perform addition
        result = performAddition(num1, num2);
        resultNegative = neg1; // result has same sign as inputs
    } else {
        // Different signs - perform subtraction
        if (num1 < num2) {
            std::swap(num1, num2);
            resultNegative = !neg1;
        } else {
            resultNegative = neg1;
        }
        result = performSubtraction(num1, num2);
    }
    
    // Re-insert decimal point
    if (dotPos != std::string::npos) {
        result.insert(result.begin() + (result.size() - (num1.size() - dotPos)), '.');
    }
    
    // Clean up result
    size_t newDotPos = result.find('.');
    result = cleanResult(result, newDotPos);
    
    // Handle zero result
    if (result.empty() || result == "0") {
        return "0";
    }
    
    // Add sign if needed
    if (resultNegative) {
        result = "-" + result;
    }
    
    return result;
}

void Calculator::processNumbersFromFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    std::string line;
    const std::string fixedNumber = "-123.456";
    
    while (std::getline(file, line)) {
        // Remove any whitespace from the line
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        
        if (line.empty()) continue;
        
        std::cout << "Processing: " << line << std::endl;
        
        if (isValidDouble(line)) {
            std::cout << "Valid double: " << line << std::endl;
            
            // Perform addition with fixed number
            std::string sum = addStrings(line, fixedNumber);
            std::cout << "Sum with " << fixedNumber << ": " << sum << std::endl;
        } else {
            std::cout << "Invalid double: " << line << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    file.close();
}
