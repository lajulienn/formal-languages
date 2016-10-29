#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <string>

struct RegularExpression {
    RegularExpression(int subword, std::vector<int> prefix, std::vector<int> suffix, int word_length) :
            min_length_with_subword(subword),
            min_length_with_prefix(prefix),
            min_length_with_suffix(suffix),
            min_word_length(word_length) {}

    int min_length_with_subword;
    std::vector<int> min_length_with_prefix;
    std::vector<int> min_length_with_suffix;
    int min_word_length;
};

int MinWordLength(const std::string &expression, char symbol, int degree);

int main() {
    std::string expression;
    char symbol;
    int degree;
    std::cin >> expression >> symbol >> degree;

    int answer = MinWordLength(expression, symbol, degree);
    if (answer == -1) {
        std::cout << "INF" << std::endl;
        return 0;
    }
    std::cout << answer << std::endl;

    return 0;
}

/*
 * Returns either min length of word with symbol^degree as subword,
 * or -1 if there's no suitable word in the language.
 */
int MinWordLength(const std::string &expression, char symbol, int degree) {
    std::stack<RegularExpression> stack;
    std::set<char> alphabet = {'a', 'b', 'c'};

    for (int step = 0; step < expression.size(); ++step) {
        char token = expression[step];
        if (alphabet.find(token) != alphabet.end()) {
            std::vector<int> prefix_len(degree, -1);
            std::vector<int> suffix_len(degree, -1);
            int subword_len = -1;
            int min_word_len = 1;
            if (token == symbol) {
                prefix_len[1] = 1;
                suffix_len[1] = 1;
                if (degree == 1) {
                    subword_len = 1;
                }
            }
            stack.emplace(subword_len, prefix_len, suffix_len, min_word_len);
        } else if (token == '1') {
            std::vector<int> prefix_len(degree, -1);
            std::vector<int> suffix_len(degree, -1);
            int subword_len = -1;
            int min_word_len = 0;
            stack.emplace(subword_len, prefix_len, suffix_len, min_word_len);
        } else if (token == '*') {
            if (stack.empty()) {
                perror("Too few operands.");
            }
            RegularExpression expression = stack.top();
            if (expression.min_length_with_prefix[1] == 1) {
                stack.pop();
                expression.min_length_with_subword = degree;
                for (int i = 1; i < degree; ++i) {
                    expression.min_length_with_prefix[i] = i;
                    expression.min_length_with_suffix[i] = i;
                }
                expression.min_word_length = 0;
                stack.push(expression);
            }
        } else if (token == '+' || token == '.') {
            if (stack.size() < 2) {
                perror("Too few operands.");
            }
            RegularExpression right_operand = stack.top();
            stack.pop();
            RegularExpression left_operand = stack.top();
            stack.pop();

            int subword_len = -1;

            std::vector<int> prefix_len(degree, -1);
            std::vector<int> suffix_len(degree, -1);

            int min_word_len;

            if (token == '+') {
                if (left_operand.min_length_with_subword == -1) {
                    if (right_operand.min_length_with_subword != -1) {
                        subword_len = right_operand.min_length_with_subword;
                    }
                } else if (right_operand.min_length_with_subword == -1) {
                    if (left_operand.min_length_with_subword != -1) {
                        subword_len = left_operand.min_length_with_subword;
                    }
                } else {
                    subword_len = std::min(left_operand.min_length_with_subword,
                                           right_operand.min_length_with_subword);
                }

                min_word_len = std::min(left_operand.min_word_length, right_operand.min_word_length);

                for (int i = 1; i < degree; ++i) {
                    if (left_operand.min_length_with_prefix[i] == -1) {
                        if (right_operand.min_length_with_prefix[i] != -1) {
                            prefix_len[i] = right_operand.min_length_with_prefix[i];
                        }
                    } else if (right_operand.min_length_with_prefix[i] == -1) {
                        if (left_operand.min_length_with_prefix[i] != -1) {
                            prefix_len[i] = left_operand.min_length_with_prefix[i];
                        }
                    } else {
                        prefix_len[i] = std::min(left_operand.min_length_with_prefix[i],
                                                 right_operand.min_length_with_prefix[i]);
                    }

                    if (left_operand.min_length_with_suffix[i] == -1) {
                        if (right_operand.min_length_with_suffix[i] != -1) {
                            suffix_len[i] = right_operand.min_length_with_suffix[i];
                        }
                    } else if (right_operand.min_length_with_suffix[i] == -1) {
                        if (left_operand.min_length_with_suffix[i] != -1) {
                            suffix_len[i] = left_operand.min_length_with_suffix[i];
                        }
                    } else {
                        suffix_len[i] = std::min(left_operand.min_length_with_suffix[i],
                                                 right_operand.min_length_with_suffix[i]);
                    }
                }
            } else if (token == '.') {
                if (left_operand.min_length_with_subword == -1) {
                    if (right_operand.min_length_with_subword != -1) {
                        subword_len = right_operand.min_length_with_subword + left_operand.min_word_length;
                    }
                } else if (right_operand.min_length_with_subword == -1) {
                    if (left_operand.min_length_with_subword != -1) {
                        subword_len = left_operand.min_length_with_subword + right_operand.min_word_length;
                    }
                } else {
                    subword_len = left_operand.min_length_with_subword + right_operand.min_length_with_subword;
                }

                min_word_len = left_operand.min_word_length + right_operand.min_word_length;

                for (int i = 1; i < degree; ++i) {
                    int len_with_subword_on_joint;
                    int left_part = left_operand.min_length_with_suffix[i];
                    int right_part = right_operand.min_length_with_prefix[degree - i];
                    if (left_part != -1 && right_part != -1) {
                        len_with_subword_on_joint = left_part + right_part;
                        if (len_with_subword_on_joint < subword_len || subword_len == -1) {
                            subword_len = len_with_subword_on_joint;
                        }
                    }
                    if (left_operand.min_length_with_prefix[i] != -1) {
                        prefix_len[i] = left_operand.min_length_with_prefix[i] + right_operand.min_word_length;
                    }

                    if (right_operand.min_length_with_suffix[i] != -1) {
                        suffix_len[i] = right_operand.min_length_with_suffix[i] + left_operand.min_word_length;
                    }

                    if (left_operand.min_length_with_prefix[i] == i) {
                        for (int j = 1; i + j < degree; ++j) {
                            if (right_operand.min_length_with_prefix[j] != -1) {
                                prefix_len[i + j] = i + right_operand.min_length_with_prefix[j];
                            }
                        }
                    }
                    if (right_operand.min_length_with_suffix[i] == i) {
                        for (int j = 1; i + j < degree; ++j) {
                            if (left_operand.min_length_with_suffix[j] != -1) {
                                suffix_len[i + j] = i + left_operand.min_length_with_suffix[j];
                            }
                        }
                    }
                }
            }
            stack.emplace(subword_len, prefix_len, suffix_len, min_word_len);
        } else if (token == ' ') {
            continue;
        } else {
            perror("Unknown symbol.");
        }
    }
    if (stack.size() != 1) {
        perror("Wrong number of operands.");
    }
    RegularExpression answer = stack.top();
    if (degree == 0) {
        return answer.min_word_length;
    } else {
        return answer.min_length_with_subword;
    }
}