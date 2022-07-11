#include "SAPPOROBDD/include/ZBDD.h"
#include "sbdd_helper/SBDD_helper.h"

// Delete commentout if necessary.
//#include "tdzdd/DdEval.hpp"
#include "tdzdd/DdSpec.hpp"
//#include "tdzdd/DdSpecOp.hpp"
#include "tdzdd/DdStructure.hpp"

// Comment out if your program does not need the following headers.
#include "tdzdd/spec/SizeConstraint.hpp"
#include "tdzdd/eval/ToZBDD.hpp"
#include "tdzdd/spec/SapporoZdd.hpp"

// #include <iostream>
#include <vector>
#include <list>
#include <algorithm>

using namespace tdzdd;
using namespace sbddh;
using namespace std;

// Function to generate solution space without considering overlapped
ZBDD getOverlapped(int m, int n, int K, vector<vector<int>> carInfo) {
    // All placements of all cars
    vector<vector<vector<int>>> allPlacementsOfAllCars;

    for (int k = 0; k < K; k++) {
        // All placements of a car
        vector<vector<int>> allPlacementsOfCar;

        if (carInfo.at(k).at(0) == 0) {
            // If the car is horizontal
            for (int i = 0; i < m * n; i++) {
                // Placement of a car
                vector<int> placementOfCar(carInfo.at(k).at(1));
                // Whether the car is correctly positioned
                bool isValid = true;

                // Add a square where the car is located
                for (int j = 0; j < placementOfCar.size(); j++) {
                    // The case that a car is split or overflows the field
                    if ((j > 0 && (i + j) % n == 0) || i + j >= m * n) {
                        isValid = false;
                        break;
                    }

                    placementOfCar.at(j) = (i + j + 1) + m * n * k;
                }

                if (isValid) {
                    allPlacementsOfCar.push_back(placementOfCar);
                }
            }
        } else if (carInfo.at(k).at(0) == 1) {
            // If the car is vertical
            for (int i = 0; i < m * n; i++) {
                // Placement of a car
                vector<int> placementOfCar(carInfo.at(k).at(1));
                // Whether the car is correctly positioned
                bool isValid = true;

                // Add a square where the car is located
                for (int j = 0; j < placementOfCar.size(); j++) {
                    // The case that a car is split or overflows the field
                    if (i + n * j >= m * n) {
                        isValid = false;
                        break;
                    }

                    placementOfCar.at(j) = (i + n * j + 1) + m * n * k;
                }

                if (isValid) {
                    allPlacementsOfCar.push_back(placementOfCar);
                }
            }
        } else {
            // TODO: Output error
        }

        allPlacementsOfAllCars.push_back(allPlacementsOfCar);
    }

    // generate a vector contains ZDD represent the position of a car
    vector<ZBDD> ZDDVec;
    for (vector<vector<int>> allPlacements : allPlacementsOfAllCars) {
        ZBDD z = getSingleSet(allPlacements.at(0));
        for (int i = 1; i < allPlacements.size(); i++) {
            z = operator+(z, getSingleSet(allPlacements.at(i)));
        }

        ZDDVec.push_back(z);
    }

    // generate ZDD without considering overlap
    ZBDD overlappedZDD = ZDDVec.at(0);
    for (int i = 1; i < ZDDVec.size(); i++) {
        overlappedZDD = operator*(overlappedZDD, ZDDVec.at(i));
    }

    return overlappedZDD;
}

// Function to generate a ZDD represents node number per square
ZBDD getNodesPerSquare(int m, int n, int K) {
    // ZDD represents node number per square
    ZBDD nodesPerSquare;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            // a vector represents node number corresponding to a square
            vector<int> oneSquare(K);
            for (int k = 0; k < K; k++) {
                // NOTE: Node numbers begin with 1!!
                oneSquare.at(k) = ((i + 1) + m * j + m * n * k);
            }

            if (i == 0 && j == 0) {
                nodesPerSquare = getSingleSet(oneSquare);
            } else {
                nodesPerSquare = operator+(nodesPerSquare, getSingleSet(oneSquare));
            }
        }
    }

    return nodesPerSquare;
}

// Function to find the elements of F which intersect S and the number of elements contained by intersection is less than or equal to k
ZBDD selectWithLimitByIntersection(ZBDD F, ZBDD S, int k) {
    if (k < 0) {
        return ZBDD(0);
    } else if (F == ZBDD(0) || F == ZBDD(1) || S == ZBDD(0) || S == ZBDD(1)) {
        return F;
    } else {
        int t1 = F.Top();
        int t2 = S.Top();

        if (t1 == t2) {
            // If the top-level node of F and S are the same
            ZBDD F0 = F.OffSet(t1);
            ZBDD F1 = F.OnSet0(t1);
            ZBDD S0 = S.OffSet(t1);
            ZBDD S1 = S.OnSet0(t1);

            ZBDD F0S0 = selectWithLimitByIntersection(F0, S0, k);
            ZBDD F0S1 = selectWithLimitByIntersection(F0, S1, k);
            ZBDD F1S0 = selectWithLimitByIntersection(F1, S0, k);
            ZBDD F1S1 = selectWithLimitByIntersection(F1, S1, k - 1);

            return operator+(operator&(F0S0, F0S1), operator*(getSingleton(t1), operator&(F1S0, F1S1)));
        } else if (t1 < t2) {
            // If the top-level node of S is greater than F's
            ZBDD S0 = S.OffSet(t2);
            ZBDD S1 = S.OnSet0(t2);

            ZBDD FS0 = selectWithLimitByIntersection(F, S0, k);
            ZBDD FS1 = selectWithLimitByIntersection(F, S1, k);

            return operator&(FS0, FS1);
        } else {
            // If the top-level node of F is greater than S's
            ZBDD F0 = F.OffSet(t1);
            ZBDD F1 = F.OnSet0(t1);

            ZBDD F0S = selectWithLimitByIntersection(F0, S, k);
            ZBDD F1S = selectWithLimitByIntersection(F1, S, k);

            return operator+(F0S, operator*(getSingleton(t1), F1S));
        }
    }
}

// Function to generate the solution space
ZBDD getSolutionSpace(int m, int n, int K, vector<vector<int>> carInfo) {
    ZBDD overlapped = getOverlapped(m, n, K, carInfo);
    ZBDD perSquare = getNodesPerSquare(m, n, K);
    return selectWithLimitByIntersection(overlapped, perSquare, 1);
}

// Functions for transitions
ZBDD remove(ZBDD F) {
    if (F == ZBDD(0) || F == ZBDD(1)) {
        return ZBDD(0);
    } else {
        int t = F.Top();
        ZBDD F0 = F.OffSet(t);
        ZBDD F1 = F.OnSet0(t);

        return operator+(operator+(F1, remove(F0)), operator*(getSingleton(t), remove(F1)));
    }
}

ZBDD add(ZBDD F, list<int> A) {
    if (F == ZBDD(0)) {
        return ZBDD(0);
    } else if (F == ZBDD(1)) {
        auto itrFirst = A.begin();
        ZBDD z = getSingleton(*itrFirst);

        for(auto itr = A.begin(); itr != A.end(); ++itr) {
            z = operator+(z, getSingleton(*itr));
        }

        return z;
    } else {
        int t = F.Top();
        ZBDD F0 = F.OffSet(t);
        ZBDD F1 = F.OnSet0(t);

        A.remove(t);

        return operator+(add(F0, A), operator*(getSingleton(t), operator+(F0, add(F1, A))));
    }
}

ZBDD swap(ZBDD F, list<int> A) {
    if (F == ZBDD(0) || F == ZBDD(1)) {
        return ZBDD(0);
    } else {
        int t = F.Top();
        ZBDD F0 = F.OffSet(t);
        ZBDD F1 = F.OnSet0(t);

        A.remove(t);

        return operator+(operator+(swap(F0, A), add(F1, A)), operator*(getSingleton(t), operator+(swap(F1, A), remove(F0))));
    }
}

// Function to perform one transition
ZBDD transition(ZBDD F, list<int> V, ZBDD FSol, ZBDD FPrev) {
    return operator-(operator&(swap(F, V), FSol), FPrev);
}

int main() {
    // m, n, K denote the number of rows, columns, and cars, respectively
    int m, n, K;
    cin >> m >> n >> K;

    // ans represents node number of the answer
    int iAns, jAns, kAns;
    cin >> iAns >> jAns >> kAns;
    int ans = jAns + n * (iAns - 1) + m * n * (kAns - 1);

    // Hold the direction and length of cars
    // HACK: The method of specifying the size of a vector is (maybe) not good
    vector<vector<int>> carInfo(K, vector<int>(2));
    for (int k = 0; k < K; k++) {
        cin >> carInfo.at(k).at(0) >> carInfo.at(k).at(1);
    }

    // Memory allocation
    BDD_Init(1024, 1024 * 1024 * 1024);

    // Vector of node numbers
    list<int> V;
    // Allocate variables to be used
    for (int i = 0; i < m * n * K; i++) {
        BDD_NewVar();
        V.push_back(i + 1);
    }

    // Construct initial ZDD
    vector<int> initialVec;
    // NOTE: Rows are denoted as i, columns as j, so j is on the outside
    // NOTE: The given initial state must be a feasible solution
    // TODO: Change to detect when input is not a feasible solution
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            string square;
            cin >> square;

            if (square == "-") {
                continue;
            } else {
                // Calculate the element number from the square and car number
                // NOTE: Node numbers begin with 1
                initialVec.push_back((j + 1) + n * i + m * n * (stoi(square) - 1));
            }
        }
    }
    ZBDD F = getSingleSet(initialVec);

    // Generate solution space
    ZBDD FSol = getSolutionSpace(m, n, K, carInfo);

    // count represent the number of transitions
    int count = 0;
    // FPred represent previous transition
    ZBDD FPrev = ZBDD(0);
    while (F != ZBDD(0)) {
        ZBDD target = F.OnSet(ans);
        if (target != ZBDD(0)) {
            // If the target solution is reached
            cout << count << endl;
            cout << ZStr(target) << endl;
            return 0;
        } else {
            ZBDD FTmp = F;
            F = transition(F, V, FSol, FPrev);
            FPrev = FTmp;
            count++;

            // FIXME: test
            // cout << ZStr(F) << endl;
        }
    }

    cout << "Cannot reach the answer..." << endl;

    return 0;
}
