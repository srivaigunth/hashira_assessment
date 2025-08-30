#include <bits/stdc++.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// ============================================================================
// BigInt Class Definition & Implementation
// ============================================================================

class BigInt {
public:
    // Constructors
    BigInt(long long n = 0);
    BigInt(string s);

    // Assignment
    BigInt& operator=(const BigInt& other);

    // Unary operators
    BigInt operator-() const;

    // Binary arithmetic operators
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;

    // Comparison operators
    bool operator<(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;

    // To string
    string toString() const;

private:
    string value;
    bool is_negative;

    // Helper functions
    static string add(string a, string b);
    static string subtract(string a, string b);
    static string multiply(string a, string b);
    static pair<string, string> divide(string a, string b);
    static bool is_smaller(const string& a, const string& b);
    static string trim(string s);
};

// Stream insertion operator for easy printing
ostream& operator<<(ostream& os, const BigInt& bi);


// --- BigInt Implementation ---

BigInt::BigInt(long long n) {
    if (n == 0) {
        value = "0";
        is_negative = false;
        return;
    }
    if (n < 0) {
        is_negative = true;
        n = -n;
    } else {
        is_negative = false;
    }
    value = to_string(n);
}

BigInt::BigInt(string s) {
    if (s.empty() || s == "0") {
        value = "0";
        is_negative = false;
        return;
    }
    if (s[0] == '-') {
        is_negative = true;
        value = s.substr(1);
    } else {
        is_negative = false;
        value = s;
    }
    value = trim(value);
}

BigInt& BigInt::operator=(const BigInt& other) {
    value = other.value;
    is_negative = other.is_negative;
    return *this;
}

string BigInt::trim(string s) {
    int i = 0;
    while (i < s.length() - 1 && s[i] == '0') {
        i++;
    }
    return s.substr(i);
}

bool BigInt::is_smaller(const string& a, const string& b) {
    if (a.length() < b.length()) return true;
    if (a.length() > b.length()) return false;
    return a < b;
}

string BigInt::add(string a, string b) {
    string sum = "";
    int i = a.length() - 1, j = b.length() - 1, carry = 0;
    while (i >= 0 || j >= 0 || carry) {
        int current_sum = carry;
        if (i >= 0) current_sum += a[i--] - '0';
        if (j >= 0) current_sum += b[j--] - '0';
        sum += to_string(current_sum % 10);
        carry = current_sum / 10;
    }
    reverse(sum.begin(), sum.end());
    return sum;
}

string BigInt::subtract(string a, string b) {
    if (is_smaller(a, b)) swap(a, b);
    string diff = "";
    int i = a.length() - 1, j = b.length() - 1, borrow = 0;
    while (j >= 0) {
        int sub = (a[i] - '0') - (b[j] - '0') - borrow;
        if (sub < 0) {
            sub += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        diff += to_string(sub);
        i--;
        j--;
    }
    while (i >= 0) {
        int sub = (a[i] - '0') - borrow;
        if (sub < 0) {
            sub += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        diff += to_string(sub);
        i--;
    }
    reverse(diff.begin(), diff.end());
    return trim(diff);
}

string BigInt::multiply(string a, string b) {
    if (a == "0" || b == "0") return "0";
    vector<int> res(a.length() + b.length(), 0);
    int i_n1 = 0, i_n2 = 0;
    for (int i = a.length() - 1; i >= 0; i--) {
        int carry = 0;
        int n1 = a[i] - '0';
        i_n2 = 0;
        for (int j = b.length() - 1; j >= 0; j--) {
            int n2 = b[j] - '0';
            int sum = n1 * n2 + res[i_n1 + i_n2] + carry;
            carry = sum / 10;
            res[i_n1 + i_n2] = sum % 10;
            i_n2++;
        }
        if (carry > 0) res[i_n1 + i_n2] += carry;
        i_n1++;
    }
    int i = res.size() - 1;
    while (i >= 0 && res[i] == 0) i--;
    if (i == -1) return "0";
    string s = "";
    while (i >= 0) s += to_string(res[i--]);
    return s;
}

pair<string, string> BigInt::divide(string dividend, string divisor) {
    if (divisor == "0") throw runtime_error("Division by zero");
    if (is_smaller(dividend, divisor)) return {"0", dividend};

    string quotient = "";
    string current = "";
    for (char d : dividend) {
        current += d;
        current = trim(current);
        int x = 0;
        while (!is_smaller(current, divisor)) {
            current = subtract(current, divisor);
            x++;
        }
        quotient += to_string(x);
    }
    return {trim(quotient), trim(current)};
}

BigInt BigInt::operator-() const {
    BigInt temp = *this;
    if (temp.value != "0") {
        temp.is_negative = !temp.is_negative;
    }
    return temp;
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (is_negative == other.is_negative) {
        BigInt res(add(value, other.value));
        res.is_negative = is_negative;
        return res;
    }
    if (is_smaller(value, other.value)) {
        BigInt res(subtract(other.value, value));
        res.is_negative = other.is_negative;
        return res;
    }
    BigInt res(subtract(value, other.value));
    res.is_negative = is_negative;
    return res;
}

BigInt BigInt::operator-(const BigInt& other) const {
    return *this + (-other);
}

BigInt BigInt::operator*(const BigInt& other) const {
    BigInt res(multiply(value, other.value));
    if (value != "0" && other.value != "0") {
        res.is_negative = (is_negative != other.is_negative);
    }
    return res;
}

BigInt BigInt::operator/(const BigInt& other) const {
    if (other.value == "0") throw runtime_error("Division by zero");
    auto res_pair = divide(value, other.value);
    BigInt res(res_pair.first);
    if (value != "0" && other.value != "0") {
        res.is_negative = (is_negative != other.is_negative);
    }
    return res;
}

BigInt BigInt::operator%(const BigInt& other) const {
    if (other.value == "0") throw runtime_error("Division by zero");
    auto res_pair = divide(value, other.value);
    BigInt res(res_pair.second);
    res.is_negative = this->is_negative;
    return res;
}

bool BigInt::operator<(const BigInt& other) const {
    if (is_negative != other.is_negative) return is_negative;
    if (is_negative) return is_smaller(other.value, value);
    return is_smaller(value, other.value);
}

bool BigInt::operator>(const BigInt& other) const { return other < *this; }
bool BigInt::operator<=(const BigInt& other) const { return !(*this > other); }
bool BigInt::operator>=(const BigInt& other) const { return !(*this < other); }
bool BigInt::operator==(const BigInt& other) const { return is_negative == other.is_negative && value == other.value; }
bool BigInt::operator!=(const BigInt& other) const { return !(*this == other); }

string BigInt::toString() const {
    if (value == "0") return "0";
    return (is_negative ? "-" : "") + value;
}

ostream& operator<<(ostream& os, const BigInt& bi) {
    os << bi.toString();
    return os;
}

// ============================================================================
// Main Application Logic
// ============================================================================

struct Share {
    int x;
    BigInt y;
    int original_index;
};

// Converts a string value from a given base to a BigInt
BigInt base_to_bigint(const string& value, int base) {
    BigInt result = 0;
    BigInt power = 1;

    for (int i = value.length() - 1; i >= 0; i--) {
        int digit;
        if (value[i] >= '0' && value[i] <= '9') {
            digit = value[i] - '0';
        } else if (value[i] >= 'a' && value[i] <= 'z') {
            digit = value[i] - 'a' + 10;
        } else if (value[i] >= 'A' && value[i] <= 'Z') {
            digit = value[i] - 'A' + 10;
        } else {
            throw runtime_error("Invalid character in value string for base conversion.");
        }

        if (digit >= base) {
            throw runtime_error("Digit is too large for the given base.");
        }

        result = result + (power * BigInt(digit));
        power = power * BigInt(base);
    }
    return result;
}

// Evaluates a polynomial defined by a set of k shares at a specific point eval_x
BigInt evaluate_polynomial_at(const vector<Share>& shares, int eval_x) {
    BigInt sum_num = 0;
    BigInt sum_den = 1;

    for (const auto& share_i : shares) {
        BigInt term_num = share_i.y;
        BigInt term_den = 1;

        for (const auto& share_j : shares) {
            if (share_i.x == share_j.x) continue;
            term_num = term_num * BigInt(eval_x - share_j.x);
            term_den = term_den * BigInt(share_i.x - share_j.x);
        }

        BigInt new_sum_num = (sum_num * term_den) + (term_num * sum_den);
        BigInt new_sum_den = sum_den * term_den;
        sum_num = new_sum_num;
        sum_den = new_sum_den;
    }

    if (sum_num % sum_den != BigInt(0)) {
        throw runtime_error("Polynomial evaluation resulted in a non-integer value.");
    }
    
    return sum_num / sum_den;
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <json_file>" << endl;
        return 1;
    }

    ifstream f(argv[1]);
    if (!f) {
        cerr << "Error: Could not open file " << argv[1] << endl;
        return 1;
    }
    json data = json::parse(f);

    int n = data["keys"]["n"];
    int k = data["keys"]["k"];
    vector<Share> all_shares;

    cout << "Parsing shares..." << endl;
    for (auto& [key, val] : data.items()) {
        if (key == "keys") continue;
        try {
            int x = stoi(key);
            string value_str = val["value"];
            int base = stoi(val["base"].get<string>());
            BigInt y = base_to_bigint(value_str, base);
            all_shares.push_back({x, y, x});
            cout << "  - Parsed share (" << x << ", " << y << ")" << endl;
        } catch (const exception& e) {
            cerr << "Error parsing share with key '" << key << "': " << e.what() << endl;
        }
    }

    if (all_shares.size() < k) {
        cerr << "Error: Not enough shares to reconstruct the secret. Need " << k << ", but only have " << all_shares.size() << "." << endl;
        return 1;
    }

    cout << "\nSearching for the correct polynomial (degree " << k - 1 << ")..." << endl;
    
    vector<Share> best_poly_shares;
    int max_matches = 0;

    vector<bool> v(all_shares.size());
    fill(v.begin() + all_shares.size() - k, v.end(), true);

    do {
        vector<Share> current_subset;
        for (int i = 0; i < all_shares.size(); ++i) {
            if (v[i]) {
                current_subset.push_back(all_shares[i]);
            }
        }
        
        int current_matches = 0;
        for (const auto& share_to_check : all_shares) {
            try {
                BigInt evaluated_y = evaluate_polynomial_at(current_subset, share_to_check.x);
                if (evaluated_y == share_to_check.y) {
                    current_matches++;
                }
            } catch(...) {
                continue;
            }
        }

        if (current_matches > max_matches) {
            max_matches = current_matches;
            best_poly_shares = current_subset;
        }

    } while (next_permutation(v.begin(), v.end()));

    cout << "Found a polynomial consistent with " << max_matches << " out of " << all_shares.size() << " shares." << endl;

    if (max_matches < k) {
        cerr << "Error: Could not find a consistent polynomial with at least " << k << " shares." << endl;
        return 1;
    }

    BigInt secret = evaluate_polynomial_at(best_poly_shares, 0);

    vector<int> invalid_share_indices;
    for (const auto& share : all_shares) {
        BigInt evaluated_y = evaluate_polynomial_at(best_poly_shares, share.x);
        if (evaluated_y != share.y) {
            invalid_share_indices.push_back(share.original_index);
        }
    }
    
    cout << "\n--- Results ---" << endl;
    cout << "Secret (f(0)): " << secret << endl;

    if (invalid_share_indices.empty()) {
        cout << "All provided shares are valid." << endl;
    } else {
        cout << "Invalid shares found at indices: ";
        sort(invalid_share_indices.begin(), invalid_share_indices.end());
        for (size_t i = 0; i < invalid_share_indices.size(); ++i) {
            cout << invalid_share_indices[i] << (i == invalid_share_indices.size() - 1 ? "" : ", ");
        }
        cout << endl;
    }

    return 0;
}