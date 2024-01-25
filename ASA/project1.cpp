#include <iostream>
#include <vector>
using namespace std;

struct Coordenadas {
    int line;
    int column;
    int price;
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, line, column;
    cin >> line >> column >> n;

    vector<vector<int>> results(line + 1, vector<int>(column + 1));

    for (int i = 0; i < n; i++) {
        Coordenadas c;
        cin >> c.line >> c.column >> c.price;
        if (c.line <= line && c.column <= column) {
            results[c.line][c.column] = c.price;
        }
        if (c.column != c.line && c.column <= line && c.line <= column) {
            results[c.column][c.line] = c.price;
        }
    }

    for (int i = 1; i <= line; i++) {
        for (int j = 1; j <= column; j++) {

            int results_aux = max(results[i - 1][j], results[i][j - 1]);

            for (int k = 1; k <= i / 2; k++) {
                results_aux = max(results_aux, results[k][j] + results[i - k][j]);
            }
            for (int k = 1; k <= j / 2; k++) {
                results_aux = max(results_aux, results[i][k] + results[i][j - k]);
            }

            results[i][j] = max(results_aux, results[i][j]);
        }
    }
    cout << results[line][column] << "\n";
    return 0;
}