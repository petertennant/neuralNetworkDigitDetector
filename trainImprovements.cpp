#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <math.h>
#include <time.h>
#include <random>
using namespace std;
//https://www.kaggle.com/datasets/oddrationale/mnist-in-csv?resource=download
const int INPUTS = 784;
const int OUTPUTS = 10;
const int HL_SIZE = 64;
const int HL_AMOUNT =1;
struct layerb {
    float bias[HL_SIZE];
    float weights[HL_SIZE][INPUTS];
};
struct layerc {
    float bias[HL_AMOUNT][HL_SIZE];
    float weights[HL_AMOUNT][HL_SIZE][HL_SIZE];
};
struct layerd {
    float bias[OUTPUTS];
    float weights[OUTPUTS][HL_SIZE];
};
struct nobes {
    layerb b;
    layerc c;
    layerd d;
};

float squisshification(float x);
void runNetwork(int picture[INPUTS], float (&hiddenLayerBrightness)[HL_AMOUNT+1][HL_SIZE], float (&results)[OUTPUTS], nobes* nobes);
nobes* randomizeNobes(nobes* nobes);
void storeNobeSettings (nobes* nobes);
nobes* fetchNobeSettings (string filename);
void recievePictureFile(string filename, int (&picture)[100][INPUTS], int (&digit)[100]);
nobes* trainOnABatch(string filename, nobes* currentNobes);
nobes* backpropagation(float results[OUTPUTS], int trueDigit, float hiddenLayerBrightness[HL_AMOUNT+1][HL_SIZE], float multipler, int picture[INPUTS], nobes* currentSettings);
float calculateCost(int actual, float results[OUTPUTS]);
nobes* applyAdjustments(nobes* desired, nobes* current);
void testmode();

int main() {
    srand(time(NULL));
    int counter =0;
    nobes* currentSettings;
    nobes* deletor;

    //testmode();
    currentSettings = fetchNobeSettings("settings2.txt");
    currentSettings = randomizeNobes(currentSettings);
    //cout << "Randomized Settings" << endl;
    for(int j=0; j<200000; j++) {
        for(int p=0; p<10000; p++) {
            for(int i=0; i<295; i++) {
                string filething = "subsetin" + to_string(i) + ".txt";
                if(!(rand()%100000)) {
                    filething = "subsetin" + to_string(i) + ".txt";
                } else {
                    cout << counter << " cost: ";
                    counter++;
                    filething = "custom" + to_string(rand()%29) + ".txt";
                }
                //deletor=currentSettings;
                currentSettings = trainOnABatch(filething, currentSettings);
                //delete [] deletor;
                //cout << currentSettings.b.weights[0][0] << endl;
                if(i%100 == 0) {
                    storeNobeSettings(currentSettings);
                }
            }
        }

        string line = "";
        ifstream mnist;
        mnist.open("mnist_train.csv");
        string filename = "subsetin0.txt";
        int filenum = 0;
        for(int p=0; p<600; p++) {
            ofstream filewrite;
            filewrite.open(filename);
            for(int j=0; j<100; j++) {
                if(mnist.is_open()) {
                    getline(mnist,line);
                }
                if(rand()%2 || 1) {
                    int num = 0;
                    int testnum = 2;
                    int counter = 0;
                    string totalstr = "";
                    line = line + ",";
                    filewrite << line.at(0);
                    while (testnum<line.length()) {
                        while (line.at(testnum) != ',') {
                            testnum++;
                            counter++;
                        }
                        for(int i=0; i<counter; i++) {
                            totalstr = totalstr+line.at(testnum-counter+i);
                        }
                        if(stoi(totalstr) < 100) {
                            filewrite << " " << 0;
                        } else {
                            filewrite << " " << 1;
                        }
                        totalstr = "";
                        testnum +=2;
                        num ++;
                        counter = 1;


                    }
                    filewrite << endl;
                } else {
                    j--;
                }
            }
            filewrite.close();
            filenum ++;
            filename = "subsetin" + to_string(filenum) + ".txt";

        }
        //mnist.close();

    }

    //storeNobeSettings(currentSettings);
    /*recievePictureFile("subset0.txt", picture, actualdigit);
    runNetwork(picture[0], layer, results, currentSettings);
    for(int i=0; i<10; i++){
        cout << results[i] << endl;
    }*/
}
void testmode() {
    nobes* currentSettings;
    currentSettings = fetchNobeSettings("settings1.txt");
    int picture[100][INPUTS];
    int digit[100];
    float results[OUTPUTS];
    recievePictureFile("testset2.txt", picture, digit) ;
    int counter = 0;
    while(1) {
        float grabge[HL_AMOUNT+1][HL_SIZE];
        runNetwork(picture[counter], grabge, results, currentSettings);
        for (int i=0; i<10; i++) {
            cout << i << " " << results[i] << endl;
        }
        cout << digit[counter] << endl;
        counter++;
        system("pause");
    }
}
nobes* randomizeNobes(nobes* nobes) {

    for(int j=0; j<HL_SIZE; j++) {
        for(int i=0; i<INPUTS; i++) {

            nobes->b.weights[j][i] = (rand()%100)/100.0-.5;
            //nobes.b.weights[j][i] = (rand()%100)/100.0-.5;
        }
        nobes->b.bias[j] = (rand()%100)/100.0-.5;
    }
    for(int p=0; p<HL_AMOUNT; p++) {
        for(int j=0; j<HL_SIZE; j++) {
            for(int i=0; i<HL_SIZE; i++) {
                nobes->c.weights[p][j][i] = (rand()%100)/100.0-.5;
                //nobes.c.weights[j][i] = (rand()%100)/100.0-.5;
            }
            nobes->c.bias[p][j] = (rand()%100)/100.0-.5;
        }
    }
    for(int j=0; j<OUTPUTS; j++) {
        for(int i=0; i<HL_SIZE; i++) {
            nobes->d.weights[j][i] = (rand()%100)/100.0-.5;
            //nobes.d.weights[j][i] = (rand()%100)/100.0-.5;
        }
        nobes->d.bias[j] = (rand()%100)/100.0-.5;
    }
    return nobes;
}
void storeNobeSettings (nobes* nobes) {
    ofstream settings;
    settings.open("largerNetwork10.txt");
    for(int j=0; j<HL_SIZE; j++) {
        for(int i=0; i<INPUTS; i++) {
            settings << to_string(nobes->b.weights[j][i]) << " ";
        }
        settings << to_string(nobes->b.bias[j]) << endl;
    }
    for(int p=0; p<HL_AMOUNT; p++) {
        for(int j=0; j<HL_SIZE; j++) {
            for(int i=0; i<HL_SIZE; i++) {
                settings << to_string(nobes->c.weights[p][j][i]) << " ";
            }
            settings << to_string(nobes->c.bias[p][j]) << endl;
        }
    }
    for(int j=0; j<OUTPUTS; j++) {
        for(int i=0; i<HL_SIZE; i++) {
            settings << to_string(nobes->d.weights[j][i]) << " ";
        }
        settings << to_string(nobes->d.bias[j]) << endl;
    }
    settings.close();
}
nobes* fetchNobeSettings (string filename) {

    nobes* nobe;
    nobe = new nobes;

    ifstream settings;
    settings.open(filename);
    for(int j=0; j<HL_SIZE; j++) {
        for(int i=0; i<INPUTS; i++) {
            settings >> nobe->b.weights[j][i];
        }
        settings >> nobe->b.bias[j];
    }
    for(int p=0; p<HL_AMOUNT; p++) {
        for(int j=0; j<HL_SIZE; j++) {
            for(int i=0; i<HL_SIZE; i++) {
                settings >> nobe->c.weights[p][j][i];
            }
            settings >> nobe->c.bias[p][j];
        }
    }
    for(int j=0; j<OUTPUTS; j++) {
        for(int i=0; i<HL_SIZE; i++) {
            settings >> nobe->d.weights[j][i];
        }
        settings >> nobe->d.bias[j];
    }
    settings.close();
    return nobe;
}
float squisshification(float x) {
    float num = 1/(1+exp(-x))*100;
    num = round(num)/100;
    return num;
}
void runNetwork(int picture[INPUTS], float (&hiddenLayerBrightness)[HL_AMOUNT+1][HL_SIZE], float (&results)[OUTPUTS], nobes* nobes) {
    //Calculate layer 2 brightness
    for(int j=0; j<HL_SIZE; j++) {
        float sum = 0.0;
        for(int i=0; i<INPUTS; i++) {
            if(picture[i]==1) {
                sum += nobes->b.weights[j][i];
            }
        }
        // + nobes.b.bias[j]
        hiddenLayerBrightness[0][j] = squisshification(sum + nobes->b.bias[j]);
    }
    for(int p=0; p<HL_AMOUNT; p++) {
        for(int j=0; j<HL_SIZE; j++) {
            float sum = 0.0;
            for(int i=0; i<HL_SIZE; i++) {
                sum += hiddenLayerBrightness[p][i] * nobes->c.weights[p][j][i];
            }
            // + nobes.c.bias[j]
            hiddenLayerBrightness[p+1][j] = squisshification(sum + nobes->c.bias[p][j]);
        }
    }
    for(int j=0; j<OUTPUTS; j++) {
        float sum = 0.0;
        for(int i=0; i<HL_SIZE; i++) {
            sum += hiddenLayerBrightness[HL_AMOUNT][i] * nobes->d.weights[j][i];
        }
        // + nobes.d.bias[j]
        results[j] = squisshification(sum + nobes->d.bias[j]);
    }
}
void recievePictureFile(string filename, int (&picture)[100][INPUTS], int (&digit)[100]) {
    int actualNum = 0;
    int tempdisplay = 0;
    ifstream mnist;
    mnist.open(filename);
    if(mnist.is_open()) {
        for(int j=0; j<100; j++) {
            mnist >> digit[j];
            for(int i=0; i<INPUTS; i++) {
                mnist >> picture[j][i];
            }
        }
    }
    mnist.close();
}
nobes* trainOnABatch(string filename, nobes* currentNobes) {
    int picture[100][INPUTS];
    int digit[100];
    recievePictureFile(filename, picture, digit);
    nobes* desiredAdjustments;
    nobes* appliedAdjustments;
    appliedAdjustments = currentNobes;
    float hiddenLayerBrightness[HL_AMOUNT+1][HL_SIZE];
    float results[OUTPUTS];
    float costsum = 0.0;
    for(int i=0; i<100; i++) {
        runNetwork(picture[i], hiddenLayerBrightness, results, currentNobes);
        desiredAdjustments = backpropagation(results, digit[i], hiddenLayerBrightness, 0.01, picture[i], currentNobes);
         applyAdjustments(desiredAdjustments,appliedAdjustments);
        //calculate cost
        costsum += calculateCost(digit[i], results);
    }
    cout << costsum/100 << endl;
    //currentNobes = weightRescale(currentNobes);
    //delete [] desiredAdjustments;
    //delete desiredAdjustments;
    return appliedAdjustments;
}
nobes* applyAdjustments(nobes* desired, nobes* current) {
    for(int j=0; j<HL_SIZE; j++) {
        for(int i=0; i<INPUTS; i++) {
            current->b.weights[j][i] += desired->b.weights[j][i];

        }
        current->b.bias[j] += desired->b.bias[j];

    }
    for(int p=0; p<HL_AMOUNT; p++) {
        for(int j=0; j<HL_SIZE; j++) {
            for(int i=0; i<HL_SIZE; i++) {
                current->c.weights[p][j][i] += desired->c.weights[p][j][i];

            }
            current->c.bias[p][j] += desired->c.bias[p][j];
        }
    }
    for(int j=0; j<OUTPUTS; j++) {
        for(int i=0; i<HL_SIZE; i++) {
            current->d.weights[j][i] += desired->d.weights[j][i];

        }
        current->d.bias[j] += desired->d.bias[j];

    }
    //delete[] desired;
    return current;
}
float calculateCost(int actual, float results[OUTPUTS]) {
    float cost = 0;
    for(int j=0; j<OUTPUTS; j++) {
        if(j == actual) {
            cost += pow((results[j]-1.0),2.0);
        } else {
            cost += pow((results[j]),2.0);
        }
    }
    return cost;
}
nobes* backpropagation(float results[OUTPUTS], int trueDigit, float hiddenLayerBrightness[HL_AMOUNT+1][HL_SIZE], float multipler, int picture[INPUTS], nobes* currentSettings) {
    nobes* desiredChange = new nobes;
    *desiredChange = *currentSettings;

    float desiredBrightness[HL_AMOUNT+1][HL_SIZE];
    float offby =0;


    for(int i=0; i<HL_SIZE; i++) {
        for(int j=0; j<HL_AMOUNT+1; j++) {
            desiredBrightness[j][i] = hiddenLayerBrightness[j][i];
        }
    }
    //hiddenLayerBrightness[1][i]
    for(int i=0; i<OUTPUTS; i++) {
        if(i==trueDigit) {
            offby = (1-results[i]);
        } else {
            offby = 0-results[i];
        }
        for(int j=0; j<HL_SIZE; j++) {
            desiredChange->d.weights[i][j] = hiddenLayerBrightness[HL_AMOUNT][j]*offby*multipler;
            desiredBrightness[HL_AMOUNT][j] += currentSettings->d.weights[i][j]*offby*multipler;
        }

        desiredChange->d.bias[i] = offby*multipler;

    }
    for(int p=HL_AMOUNT; p>0; p--) {
        for(int i=0; i<HL_SIZE; i++) {
            offby = desiredBrightness[p][i] - hiddenLayerBrightness[p][i];
            desiredChange->c.bias[p-1][i] = offby*multipler;
            for(int j=0; j<HL_SIZE; j++) {
                desiredChange->c.weights[p-1][i][j] = hiddenLayerBrightness[p-1][j]*offby*multipler;
                desiredBrightness[p-1][j] += currentSettings->c.weights[p-1][i][j]*offby*multipler;
            }
        }
    }
    for(int i=0; i<HL_SIZE; i++) {
        offby = desiredBrightness[0][i] - hiddenLayerBrightness[0][i];
        desiredChange->b.bias[i] = offby*multipler;
        for(int j=0; j<INPUTS; j++) {
            desiredChange->b.weights[i][j] = (picture[j])*offby*multipler;
        }
    }
    return desiredChange;

}










