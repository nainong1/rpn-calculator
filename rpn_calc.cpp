#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <limits>
#include <stack>
#include <cctype>
#include <algorithm>
#include <stdexcept>

class RPNCalculator {
private:
    std::vector<double> stack_;
    std::vector<std::string> history_;

    bool isNumber(const std::string& token) {
        if (token.empty()) return false;
        size_t start = 0;
        if (token[0] == '+' || token[0] == '-') {
            if (token.size() == 1) return false;
            start = 1;
        }
        bool hasDecimal = false;
        for (size_t i = start; i < token.size(); ++i) {
            if (token[i] == '.') {
                if (hasDecimal) return false;
                hasDecimal = true;
            } else if (!std::isdigit(token[i])) {
                return false;
            }
        }
        return true;
    }

    void ensureSize(size_t required) {
        if (stack_.size() < required) {
            throw std::runtime_error("栈元素不足");
        }
    }

public:
    RPNCalculator() = default;

    void push(double value) {
        stack_.push_back(value);
    }

    double pop() {
        if (stack_.empty()) {
            throw std::runtime_error("栈为空");
        }
        double top = stack_.back();
        stack_.pop_back();
        return top;
    }

    void clear() {
        stack_.clear();
    }

    void printStack() const {
        std::cout << "栈: ";
        for (double val : stack_) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    void calculate(const std::string& operation) {
        if (operation == "+") {
            ensureSize(2);
            double b = pop();
            double a = pop();
            push(a + b);
        } else if (operation == "-") {
            ensureSize(2);
            double b = pop();
            double a = pop();
            push(a - b);
        } else if (operation == "*") {
            ensureSize(2);
            double b = pop();
            double a = pop();
            push(a * b);
        } else if (operation == "/") {
            ensureSize(2);
            double b = pop();
            double a = pop();
            if (b == 0) {
                push(a);
                push(b);
                throw std::runtime_error("除零错误");
            }
            push(a / b);
        } else if (operation == "sqrt") {
            ensureSize(1);
            double a = pop();
            if (a < 0) {
                push(a);
                throw std::runtime_error("负数不能开平方");
            }
            push(std::sqrt(a));
        } else if (operation == "^") {
            ensureSize(2);
            double b = pop();
            double a = pop();
            push(std::pow(a, b));
        } else if (operation == "fib") {
            ensureSize(1);
            double n = pop();
            if (n < 0 || std::floor(n) != n) {
                push(n);
                throw std::runtime_error("斐波那契运算需要非负整数");
            }
            int intN = static_cast<int>(n);
            if (intN > 93) {
                push(n);
                throw std::runtime_error("斐波那契数过大，可能溢出");
            }
            long long a = 0, b = 1;
            for (int i = 0; i < intN; ++i) {
                long long c = a + b;
                a = b;
                b = c;
            }
            push(static_cast<double>(a));
        } else if (operation == "hist") {
            if (history_.empty()) {
                std::cout << "没有计算历史" << std::endl;
                return;
            }
            std::cout << "计算历史:" << std::endl;
            for (size_t i = 0; i < history_.size(); ++i) {
                std::cout << i + 1 << ": " << history_[i] << std::endl;
            }
        } else {
            throw std::runtime_error("未知操作符: " + operation);
        }
    }

    double evaluate(const std::string& expr) {
        std::istringstream iss(expr);
        std::string token;
        std::vector<std::string> tokens;

        while (iss >> token) {
            tokens.push_back(token);
        }

        std::vector<double> originalStack = stack_;

        try {
            for (const std::string& t : tokens) {
                if (isNumber(t)) {
                    push(std::stod(t));
                } else {
                    calculate(t);
                }
            }
        } catch (...) {
            stack_ = originalStack;
            throw;
        }

        if (stack_.empty()) {
            throw std::runtime_error("表达式计算结果为空");
        }

        history_.push_back(expr + " => " + std::to_string(stack_.back()));
        
        return stack_.back();
    }
};

void printHelp() {
    std::cout << "支持的操作符:" << std::endl;
    std::cout << "  +   -   *   /   : 四则运算" << std::endl;
    std::cout << "  sqrt           : 平方根" << std::endl;
    std::cout << "  ^              : 幂运算" << std::endl;
    std::cout << "  fib            : 斐波那契数列" << std::endl;
    std::cout << "  clear          : 清空栈" << std::endl;
    std::cout << "  print          : 显示栈" << std::endl;
    std::cout << "  hist           : 显示历史记录" << std::endl;
    std::cout << "  help           : 显示帮助信息" << std::endl;
    std::cout << "  q              : 退出程序" << std::endl;
}

int main() {
    std::cout << "C++ RPN 计算器" << std::endl;
    std::cout << "输入表达式 (例, '5 5 +'), 或 'q' 退出, 'help' 查看帮助." << std::endl;

    RPNCalculator calc;
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        size_t start = line.find_first_not_of(" \t");
        size_t end = line.find_last_not_of(" \t");
        if (start == std::string::npos) {
            continue;
        }
        line = line.substr(start, end - start + 1);

        if (line == "q" || line == "Q") {
            break;
        } else if (line == "help") {
            printHelp();
            continue;
        } else if (line == "clear") {
            calc.clear();
            std::cout << "栈已清空" << std::endl;
            continue;
        } else if (line == "print") {
            calc.printStack();
            continue;
        }

        try {
            double result = calc.evaluate(line);
            std::cout << "结果: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "错误: " << e.what() << std::endl;
        }
    }

    return 0;
}

