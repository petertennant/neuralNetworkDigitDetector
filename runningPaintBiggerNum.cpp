/*=========================================================
Programmer  : Peter Tennant
Assignment  : Coding
Due Date    : Yesterday

Description (I.P.O)
Input: User will select color and paintbrushes with mouse input
Processing: Program will determine where the user clicked and what
color to paint where
Output: Program will output the painting

Assumptions

===========================================================*/
#include <Windows.h>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <cstdlib>
#include <time.h>

using namespace std;

const int INPUTS = 784;
const int OUTPUTS = 10;
const int HL_SIZE = 64;
const int HL_AMOUNT = 1;
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

const int SCREEN_H = 37;
const int SCREEN_W = 200;
const int TOP_BORDER_SIZE = 8;
const int FONT_W = 4;
const int FONT_H = 8;

void changeFont(int fontSize);
void printBoard(HANDLE theConsole, const int* screen);
wchar_t convertIntToCh(const int& convertingInt, WORD& color);
void convertStringToWChar(wchar_t* charArray, string stringAdd, int placement);
void setIntArrayToSingleValue(int* intArray, int value);
void click();
void setupsettings(HWND);
void topBarSetup(int* screen, string numbers7Seg[11][5], int Y_COLOR_SIZE, int X_COLOR_SIZE, int COLOR_DISTANCE_FROM_EDGE_X, int COLOR_DISTANCE_FROM_SIDE_X);
void displayNumber(int number, string numbers7Seg[11][5], int* screen, int xcoord);
void saveScreenToFile(string fileName, const int* screen);
bool openFileToScreen(string fileName, int* screen);
void topBarDisplay(int* screen, string numbers7Seg[11][5], int selectedTab, int selectedTool);
void displayColors(int* screen, const int& Y_COLOR_SIZE, const int& X_COLOR_SIZE, const int& COLOR_DISTANCE_FROM_SIDE_X, const int& background);
nobes** openThe500000NumberFile();
void runNetwork(int picture[INPUTS], float (&hiddenLayerBrightness)[HL_AMOUNT+1][HL_SIZE], float (&results)[OUTPUTS], nobes* nobes);
void saveToFile(int* screen, int num, int counter);
float squisshification(float x);
int testmode(int, nobes**);
main() {
    //If the user runs the program for the first time then output the text
    fstream fileStream;
    nobes** nobe;
    nobe = openThe500000NumberFile();


    fileStream.close();

    //Get hwnd to use in changing the font and taking control of the computer to change the settings
    HWND hwnd = GetConsoleWindow();

    //Set up random
    srand(time(NULL));

    int *screen    = new int [SCREEN_H*SCREEN_W+1]; //Main screen array
    const int Y_COLOR_SIZE                = 3; //The size that each color of the color picker gets
    const int X_COLOR_SIZE               = 6; //The size that each color of the color picker gets
    const int COLOR_DISTANCE_FROM_EDGE_X = 1; //The distance that colors are from the edge
    const int COLOR_DISTANCE_FROM_SIDE_X = 5; //the distance that colors are from the side
    int color                            = 54; //The color of a specific pixel
    int lastKeystate                     = 0; //Whether the key clicked in the last frame
    int middleKeystate                   = 0; //Whether the middle mouse was clicked in the last frame
    int cKeystate = 0;
    int lastCoordY                       = 0; //The coord of the last pixel
    int lastCoordX                       = 0; //The coord of the last pixel
    int background                       = 0; //The background color
    int fileOptions                      = 0; //If the user clicks save or open
    int exit                             = 1; //Used to exit the main loop
    int paintSize                        = 0; //The size of the painbrush
    int selectedTool                     = 1; //The selected tool
    int lastKeystate_up                  = 0; //Used to determine presses of the up button
    int lastKeystate_down                = 0; //Used to determine presses of the down button
    int selectedTab                      = 0; //The tab selected
    string fileName                      = ""; //The name of the file
    int fileCount = 0;
    int randdigit = rand()%10;
    int digit[10] = {0,0,0,0,0,0,0,0,0,0};
    int gKeystate=0;
    string numbers7Seg[11][5] = {"###", "# #", "# #", "# #", "###",
                                 " # ", "## ", " # ", " # ", "###",
                                 "###", "  #", "###", "#  ", "###",
                                 "###", "  #", " ##", "  #", "###",
                                 "# #", "# #", "###", "  #", "  #",
                                 "###", "#  ", "###", "  #", "###",
                                 "###", "#  ", "###", "# #", "###",
                                 "###", "  #", "  #", "  #", "  #",
                                 "###", "# #", "###", "# #", "###",
                                 "###", "# #", "###", "  #", "  #",
                                 " # ", "###", " # ", "###", " # "
                                };

    //Initialize the screen to 0
    setIntArrayToSingleValue(screen, 0);

    //Create and intionalize the tab files to 0
    for(int i=0; i<5; i++) {
        ofstream tab;
        string fileTabName = to_string(i)+"tab.txt";
        tab.open(fileTabName.c_str());
        if(tab.is_open()) {
            for(int j=0; j<SCREEN_W*SCREEN_H; j++) {
                tab << "0" << "\n";
            }
        }
        tab << "\0";
        tab.close();
    }

    color = 4;
    cin.clear();
    changeFont(8);
    system("mode con: cols=200 lines=36");

    cin.clear();
    HANDLE theConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(theConsole);
    while(1) {
        for(int i=0; i<SCREEN_W; i++) {
            for(int j=9; j<SCREEN_H; j++) {
                screen[j*SCREEN_W+i] = 0;
            }
        }
        //Create the top bar and set the size of the console
        exit = 1;


        while(exit) {

            POINT curPos; //Stores coords of the mouse
            //Get coords of the mouse and set the relative to the screen
            BOOL result = GetCursorPos(&curPos);
            ScreenToClient(hwnd, &curPos);

            int xSide = floor(curPos.x/FONT_W); //The pixel the mouse is on
            int ySide = floor(curPos.y/FONT_H); //The pixel the mouse is on




            //If the mouse is clicked and in range
            if(curPos.x>0 && curPos.x<SCREEN_W*FONT_W && curPos.y>FONT_H*(TOP_BORDER_SIZE+1) && curPos.y<SCREEN_H*FONT_H && GetAsyncKeyState(0x01) == -32768) {

                screen[ySide*SCREEN_W+xSide] = color;
                //if the paintbrush is selected
                if(lastKeystate == -32768 && lastCoordX>0 && lastCoordX<SCREEN_W && lastCoordY>TOP_BORDER_SIZE && lastCoordY<SCREEN_H && selectedTool == 1) {
                    int iRounded;
                    double numChars = max(abs(xSide-lastCoordX), abs(ySide-lastCoordY));
                    if (numChars<1.0) {
                        numChars = 1.0;
                    }
                    int x=0;
                    int y=0;

                    //Draw a line from the last coord to the new coord so there are no gaps

                    for(int p=0; p <= numChars; p++) {
                        x=floor(xSide+(lastCoordX-xSide)*(p/numChars));
                        y=floor(ySide+(lastCoordY-ySide)*(p/numChars));
                        screen[y*SCREEN_W+x] = color;

                        if(x%2==0) {
                            screen[y*SCREEN_W+x+1] = color;
                        } else {
                            screen[y*SCREEN_W+x-1] = color;
                        }
                        for(int i=x-paintSize; i<x+paintSize; i++) {
                            for(int j=y-paintSize*4; j<y+paintSize*4+1; j++) {
                                if(i>=0 && i<=SCREEN_W && j>=TOP_BORDER_SIZE+1 && j<=SCREEN_H) {
                                    if(sqrt(pow(i*FONT_W-x*FONT_W,2)+pow(j*FONT_H-y*FONT_H,2))<=(paintSize)*4) {
                                        screen[j*SCREEN_W+i]=color;
                                        if(i%2==0) {
                                            screen[y*SCREEN_W+i+1] = color;

                                        } else {
                                            screen[y*SCREEN_W+i-1] = color;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            topBarDisplay(screen, numbers7Seg, selectedTab, selectedTool);
            for(int i=0; i<10; i++) {
                displayNumber(digit[i], numbers7Seg, screen, 5+(i*10));
            }

            if (GetAsyncKeyState(0x43) == 0 && cKeystate == -32768) {
                exit = 0;
                fileOptions=2;
            } else if(GetAsyncKeyState(0x47)== 0 && gKeystate == -32768) {
                exit = 0;
                fileOptions = 1;
            }
            lastCoordX = xSide;
            lastCoordY = ySide;

            lastKeystate = GetAsyncKeyState(0x01);
            cKeystate = GetAsyncKeyState(0x43);
            gKeystate = GetAsyncKeyState(0x47);
            printBoard(theConsole, screen);
        }

        //destroyScreenBuffer();
        //Get a file name and save the screen
        if(fileOptions==1) {
            for(int i =0; i<10; i++) {
                digit[i]=0;
            }
            bool isLitUp[SCREEN_W/2];
            int start = 0;
            int ender = 0;
            bool stoper=0;
            for(int i=0; i<SCREEN_W/2; i++) {
                isLitUp[i]=false;
                for(int j=10; j<SCREEN_H; j++) {
                    if(screen[j*SCREEN_W+i*2]==4) {
                        isLitUp[i]=true;
                    }
                }
            }

            int counter = 0;
            while(!stoper) {
                stoper=true;

                for(int i=ender; i<SCREEN_W; i+=2) {
                    if(isLitUp[(i-2)/2] == false && isLitUp[i/2]) {
                        start= i;
                        stoper = false;
                        break;
                    }
                }
                for(int i=start; i<SCREEN_W; i+=2) {
                    if(isLitUp[(i-2)/2] && isLitUp[i/2] == false && !stoper) {
                        ender= i;
                        break;
                    }
                }
                //if(start>stoper){
                //  system("pause");
                //}

                if(!stoper) {
                    saveToFile(screen, start, ender);
                    digit[counter]=testmode(fileCount, nobe);
                    counter++;
                }

            }
            fileCount++;

        }
    }
    for(int i=0; i<10; i++) {
        delete nobe[i];
    }
    delete[] nobe;
}
nobes** openThe500000NumberFile(){
    nobes** nobe;
    nobe = new nobes*[10];
    for(int i=0; i<10; i++) {
        nobe[i] = new nobes;
    }

    ifstream settings;
    settings.open("10NetworksCombinedLARGE.txt");
    for(int p=0; p<10; p++) {
        for(int j=0; j<HL_SIZE; j++) {
            for(int i=0; i<INPUTS; i++) {
                settings >> nobe[p]->b.weights[j][i];
            }
            settings >>nobe[p]->b.bias[j];
        }
        for(int j=0; j<HL_SIZE; j++) {
            for(int i=0; i<HL_SIZE; i++) {
                settings >> nobe[p]->c.weights[0][j][i];
            }
            settings >> nobe[p]->c.bias[0][j];
        }
        for(int j=0; j<OUTPUTS; j++) {
            for(int i=0; i<HL_SIZE; i++) {
                settings >> nobe[p]->d.weights[j][i];
            }
            settings >> nobe[p]->d.bias[j];
        }
    }
    settings.close();
    return nobe;
}
int testmode(int filecount, nobes** nobe) {




    int picture[784];
    int digit[100];
    float results[10];

    ifstream mnist;
    string filename = "tester.txt";
    mnist.open(filename);
    if(mnist.is_open()) {
        for(int i=0; i<28*28; i++) {
            mnist >> picture[i];
        }
    }
    mnist.close();
    float combinedresults[10];


    for(int i=0; i<10; i++) {
        combinedresults[i] = 0;
    }
    float grabge[HL_AMOUNT+1][HL_SIZE];


    for(int p=0; p<10; p++) {
        runNetwork(picture, grabge, results, nobe[p]);

        for (int i=0; i<10; i++) {
            combinedresults[i] += results[i];
        }
        cout << endl;
    }

    //find the biggest
    int biggest =0;
    for(int i=0; i<10; i++) {
        if(combinedresults[biggest]<combinedresults[i]) {
            biggest=i;
        }
    }
    return biggest;


}
void saveToFile(int* screen, int num, int counter) {
    ofstream filewrite;
    string filename = "tester.txt";
    filewrite.open(filename);
    bool isfill[SCREEN_H];
    for(int j=9; j<SCREEN_H; j++) {
        for(int i=num-floor((56-(counter-num))/2); i<counter+ceil((56-(counter-num))/2); i+=2) {
            if(i>=num && i<counter) {
                if(screen[j*SCREEN_W+i]==4) {
                    filewrite << 1 << " ";
                } else {
                    filewrite << 0 << " ";
                }
            } else {
                filewrite << 0 << " ";
            }
        }
    }


    filewrite.close();
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
float squisshification(float x) {
    float num = 1/(1+exp(-x))*100;
    num = round(num)/100;
    return num;
}
void topBarSetup(int* screen, string numbers7Seg[11][5], int Y_COLOR_SIZE, int X_COLOR_SIZE, int COLOR_DISTANCE_FROM_EDGE_X, int COLOR_DISTANCE_FROM_SIDE_X) {
    for(int i=0; i<TOP_BORDER_SIZE; i++) {
        for(int j=0; j<SCREEN_W; j++) {
            screen[i*SCREEN_W+j] = 70;
        }
    }

    /*
    for(int j=0; j<4; j++) {
        for(int i=0; i<2; i++) {
            for(int p=0; p<Y_COLOR_SIZE; p++) {
                for(int q=0; q<X_COLOR_SIZE; q++) {
                    screen[(i*Y_COLOR_SIZE+1+p)*SCREEN_W+(j+16+COLOR_DISTANCE_FROM_EDGE_X+COLOR_DISTANCE_FROM_SIDE_X)*X_COLOR_SIZE+q]=(j*2+i)*10+4;
                }
            }
        }
    }*/


    for(int i=0; i<SCREEN_W; i++) {
        screen[TOP_BORDER_SIZE*SCREEN_W+i] = 74;
    }
    for(int i=0; i<5; i++) {
        displayNumber(i+1, numbers7Seg, screen, i*6+210);
    }
    for(int i=0; i<5; i++) {
        displayNumber(Y_COLOR_SIZE, numbers7Seg, screen, i*6+210+50);
    }
}
void changeFont(int fontSize) {
    static CONSOLE_FONT_INFOEX  fontex;
    fontex.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetCurrentConsoleFontEx(hOut, 0, &fontex);
    fontex.dwFontSize.Y = fontSize;
    SetCurrentConsoleFontEx(hOut, NULL, &fontex);
}
void printBoard(HANDLE theConsole, const int* screen) {
    wchar_t *frame = new wchar_t[SCREEN_W*SCREEN_H+1000]; //WriteConsoleOutputAttribute takes a wide character array so i need to convert my current screen array to this array
    int counter = 0; //Counts how long a segment of color is. Important to figuring out how long the color array has to be
    WORD color;      //WriteConsoleOutputAttribute takes a WORD variable that stores the color
    DWORD garbage;   //WriteConsoleOutputAttribute has a DWORD parameter that is useless in this case
    COORD last;      //WriteConsoleOutputAttribute takes a COORD parameter that determines where to start
    last.X = 0;
    last.Y = 0;

    WORD attribute[SCREEN_W*SCREEN_H];
    for(int i=0; i<SCREEN_H; i++) {
        for(int j=0; j<SCREEN_W; j++) {
            frame[i*SCREEN_W+j] = convertIntToCh(screen[i*SCREEN_W+j], color);
            attribute[i*SCREEN_W+j] = color;

        }
    }
    WriteConsoleOutputAttribute(theConsole, &attribute[0], SCREEN_W*SCREEN_H, last, &garbage);


    //Print the frame
    frame[SCREEN_H*SCREEN_W+1] = '\0';
    WriteConsoleOutputCharacterW(theConsole, frame, SCREEN_W * SCREEN_H, { 0,0 }, &garbage);
    delete[] frame;
}
wchar_t convertIntToCh(const int& convertingInt, WORD& color) {
    /*      Red Red-Blue Blue Blue-Green Green Green-Red
    2591
    2592
    2593
    2588
    */
    //These all return characters in UTF-16 because using ascii or doing it normally didn't work
    color=0;

    if(floor(convertingInt/100)!=0) {
        color=floor(convertingInt/100)*16+128;
    }
    color+=floor((convertingInt%100)/10)+8;

    if(convertingInt%10 == 0) {
        return 0x0020;
    }
    if(convertingInt%10 == 1) {
        return 0x2591;
    }
    if(convertingInt%10 == 2) {
        return 0x2592;
    }
    if(convertingInt%10 == 3) {
        return 0x2593;
    }
    if(convertingInt%10 == 4) {
        return 0x2588;
    }

    return 0x0025;
}
void setIntArrayToSingleValue(int* intArray, int value) {
    for(int i=0; i<SCREEN_H; i++) {
        for(int j=0; j<SCREEN_W; j++) {
            intArray[i*SCREEN_W+j]=value;
        }
    }
}
bool openFileToScreen(string fileName, int* screen) {
    string line = "";
    ifstream save;
    save.open(fileName.c_str());
    if(save.is_open()) {
        for(int i=0; i<SCREEN_W*SCREEN_H; i++) {
            getline (save,line);
            screen[i]=stoi(line);
        }
        save.close();
        return false;
    }
    return true;
}

void displayNumber(int number, string numbers7Seg[11][5], int* screen, int xcoord) {
    for(int i=0; i<5; i++) {
        for(int j=0; j<3; j++) {
            if(numbers7Seg[number][i].at(j)=='#')
                screen[(i+1)*SCREEN_W+j+xcoord]=74;
            else {
                screen[(i+2)*SCREEN_W+j+xcoord]=70;
            }
        }
    }
}

void saveScreenToFile(string fileName, const int* screen) {
    ofstream save;
    save.open(fileName.c_str());
    if(save.is_open()) {
        for(int i=0; i<SCREEN_W*SCREEN_H; i++) {
            save << to_string(screen[i]) << "\n";
        }

    }
    save << "\0";
    save.close();
}


void topBarDisplay(int* screen, string numbers7Seg[11][5], int selectedTab, int selectedTool) {
    for(int i=0; i<SCREEN_W; i++) {
        for(int j=0; j<8; j++) {
            screen[j*SCREEN_W+i] = 70;
        }
    }
    for(int i=0; i<SCREEN_W; i++) {
        screen[8*SCREEN_W+i] = 74;
    }
}
