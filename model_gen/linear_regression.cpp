#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] 
             << " <csv_file> <independent_index> <dependent_index>\n";
        return 1;
    }

    string filename = argv[1];
    int x_col = atoi(argv[2]);
    int y_col = atoi(argv[3]);

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << "\n";
        return 1;
    }

    // ---------------------------
    // Read header row and extract column names
    // ---------------------------
    string header;
    getline(file, header);
    vector<string> col_names;
    stringstream ss_header(header);
    string col;
    while (getline(ss_header, col, ',')) {
        col_names.push_back(col);
    }

    if (x_col >= col_names.size() || y_col >= col_names.size()) {
        cerr << "Column index out of range in header.\n";
        return 1;
    }

    string x_name = col_names[x_col];
    string y_name = col_names[y_col];

    // ---------------------------
    // Read data
    // ---------------------------
    vector<double> x_vals, y_vals;
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string cell;
        vector<string> tokens;

        while (getline(ss, cell, ',')) {
            tokens.push_back(cell);
        }

        if (x_col >= tokens.size() || y_col >= tokens.size()) {
            cerr << "Column index out of range on line: " << line << "\n";
            return 1;
        }

        try {
            x_vals.push_back(stod(tokens[x_col]));
            y_vals.push_back(stod(tokens[y_col]));
        } catch (std::exception &e) {
            cerr << "Error converting row to numbers: " << line << "\n";
            return 1;
        }
    }

    file.close();

    if (x_vals.size() < 2) {
        cerr << "Not enough data rows for regression.\n";
        return 1;
    }

    // ---------------------------
    // Compute means
    // ---------------------------
    double sum_x = 0, sum_y = 0;
    for (size_t i = 0; i < x_vals.size(); i++) {
        sum_x += x_vals[i];
        sum_y += y_vals[i];
    }

    double mean_x = sum_x / x_vals.size();
    double mean_y = sum_y / y_vals.size();

    // ---------------------------
    // Compute slope (β1) and intercept (β0)
    // ---------------------------
    double num = 0, den = 0;
    for (size_t i = 0; i < x_vals.size(); i++) {
        num += (x_vals[i] - mean_x) * (y_vals[i] - mean_y);
        den += (x_vals[i] - mean_x) * (x_vals[i] - mean_x);
    }

    double slope = num / den;
    double intercept = mean_y - slope * mean_x;

    // ---------------------------
    // Compute R^2
    // ---------------------------
    double ss_tot = 0, ss_res = 0;
    for (size_t i = 0; i < x_vals.size(); i++) {
        double pred = intercept + slope * x_vals[i];
        ss_res += (y_vals[i] - pred) * (y_vals[i] - pred);
        ss_tot += (y_vals[i] - mean_y) * (y_vals[i] - mean_y);
    }

    double r2 = 1 - (ss_res / ss_tot);

    // ---------------------------
    // Output results
    // ---------------------------
    cout << "Linear regression result:\n";
    cout << "Independent variable: " << x_name << " (column " << x_col << ")\n";
    cout << "Dependent variable: " << y_name << " (column " << y_col << ")\n";
    cout << "Slope (β1):      " << slope << "\n";
    cout << "Intercept (β0):  " << intercept << "\n";
    cout << "R^2:             " << r2 << "\n";

    return 0;
}
