#include <cstdio>
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <string>
#include <climits>

static const int INF = INT_MAX;

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
    if (answer == INF) {
        std::cout << "INF" << std::endl;
        return 0;
    }
    std::cout << answer << std::endl;

    return 0;
}

template <typename T>
bool Minimize(T &value, const T &new_value) {
    if (new_value < value) {
        value = new_value;
        return true;
    } else {
        return false;
    }
}

int GreaterMultiple(int k, int bound) {
    if (bound % k == 0) {
        return bound;
    } else {
        return (bound / k + 1) * k;
    }
}

int MinJointLength(int degree, std::vector<int> suffix_len, std::vector<int> prefix_len) {
    int subword_len = INF;

    for (int i = 1; i < degree; ++i) {
        int len_with_subword_on_joint;
        int left_part = suffix_len[i];
        int right_part = prefix_len[degree - i];
        if (left_part != INF && right_part != INF) {
            len_with_subword_on_joint = left_part + right_part;
            if (len_with_subword_on_joint < subword_len || subword_len == INF) {
                subword_len = len_with_subword_on_joint;
            }
        }
    }

    return subword_len;
}

/*
 * Returns either min length of word with symbol^degree as subword,
 * or INF if there's no suitable word in the language.
 */
int MinWordLength(const std::string &expression, char symbol, int degree) {
    std::stack<RegularExpression> stack;
    std::set<char> alphabet = {'a', 'b', 'c'};

    for (int step = 0; step < expression.size(); ++step) {
        char token = expression[step];
        if (alphabet.find(token) != alphabet.end()) {
            std::vector<int> prefix_len(degree, INF);
            std::vector<int> suffix_len(degree, INF);
            int subword_len = INF;
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
            std::vector<int> prefix_len(degree, INF);
            std::vector<int> suffix_len(degree, INF);
            int subword_len = INF;
            int min_word_len = 0;
            stack.emplace(subword_len, prefix_len, suffix_len, min_word_len);
        } else if (token == '*') {
            if (stack.empty()) {
                perror("Too few operands.");
            }
            RegularExpression expression = stack.top();
            stack.pop();

            Minimize(expression.min_length_with_subword,
                     MinJointLength(degree, expression.min_length_with_suffix, expression.min_length_with_prefix));
            for (int k = 1; k < degree; ++k) {
                if (expression.min_length_with_prefix[k] == k) {
                    for (int i = 1; i < degree; ++i) {
                        Minimize(expression.min_length_with_suffix[i], GreaterMultiple(k, i));
                    }
                    Minimize(expression.min_length_with_subword, GreaterMultiple(k, degree));
                }
            }

            expression.min_word_length = 0;
            stack.push(expression);
        } else if (token == '+' || token == '.') {
            if (stack.size() < 2) {
                perror("Too few operands.");
            }
            RegularExpression right_operand = stack.top();
            stack.pop();
            RegularExpression left_operand = stack.top();
            stack.pop();

            int subword_len = INF;

            std::vector<int> prefix_len(degree, INF);
            std::vector<int> suffix_len(degree, INF);

            int min_word_len;

            if (token == '+') {
                if (left_operand.min_length_with_subword == INF) {
                    subword_len = right_operand.min_length_with_subword;
                } else if (right_operand.min_length_with_subword == INF) {
                    subword_len = left_operand.min_length_with_subword;
                } else {
                    subword_len = std::min(left_operand.min_length_with_subword,
                                           right_operand.min_length_with_subword);
                }

                min_word_len = std::min(left_operand.min_word_length, right_operand.min_word_length);

                for (int i = 1; i < degree; ++i) {
                    if (left_operand.min_length_with_prefix[i] == INF) {
                        prefix_len[i] = right_operand.min_length_with_prefix[i];
                    } else if (right_operand.min_length_with_prefix[i] == INF) {
                        prefix_len[i] = left_operand.min_length_with_prefix[i];
                    } else {
                        prefix_len[i] = std::min(left_operand.min_length_with_prefix[i],
                                                 right_operand.min_length_with_prefix[i]);
                    }

                    if (left_operand.min_length_with_suffix[i] == INF) {
                        suffix_len[i] = right_operand.min_length_with_suffix[i];
                    } else if (right_operand.min_length_with_suffix[i] == INF) {
                        suffix_len[i] = left_operand.min_length_with_suffix[i];
                    } else {
                        suffix_len[i] = std::min(left_operand.min_length_with_suffix[i],
                                                 right_operand.min_length_with_suffix[i]);
                    }
                }
            } else if (token == '.') {
                int left_subword_len = left_operand.min_length_with_subword + right_operand.min_word_length;
                int right_subword_len = right_operand.min_length_with_subword + left_operand.min_word_length;
                if (left_operand.min_length_with_subword == INF) {
                    if (right_operand.min_length_with_subword != INF) {
                        subword_len = right_subword_len;
                    }
                } else if (right_operand.min_length_with_subword == INF) {
                    if (left_operand.min_length_with_subword != INF) {
                        subword_len = left_subword_len;
                    }
                } else {
                    subword_len = std::min(left_subword_len, right_subword_len);
                }

                min_word_len = left_operand.min_word_length + right_operand.min_word_length;
                Minimize(subword_len, MinJointLength(degree, left_operand.min_length_with_suffix,
                                                                   left_operand.min_length_with_prefix));

                for (int i = 1; i < degree; ++i) {
                    if (left_operand.min_length_with_prefix[i] != INF) {
                        prefix_len[i] = left_operand.min_length_with_prefix[i] + right_operand.min_word_length;
                    }

                    if (right_operand.min_length_with_suffix[i] != INF) {
                        suffix_len[i] = right_operand.min_length_with_suffix[i] + left_operand.min_word_length;
                    }

                    if (left_operand.min_length_with_prefix[i] == i) {
                        for (int j = 1; i + j < degree; ++j) {
                            if (right_operand.min_length_with_prefix[j] != INF) {
                                Minimize(prefix_len[i + j], i + right_operand.min_length_with_prefix[j]);
                            }
                        }
                    }
                    if (right_operand.min_length_with_suffix[i] == i) {
                        for (int j = 1; i + j < degree; ++j) {
                            if (left_operand.min_length_with_suffix[j] != INF) {
                                Minimize(suffix_len[i + j], i + left_operand.min_length_with_suffix[j]);
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