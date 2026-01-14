#include <iostream>
#include <opencv2/opencv.hpp>  // use images, b&w, pixel color value
using namespace std;
#include <sys/ioctl.h>
#include <unistd.h>
#include <fstream>

//How this works:
//takes an image and changes it to black and white
//iterates through pixels and finds its color value 0-255 to assign a char of similar color density (0 = " " and 255 = "@")
//if large image adjust block size ex. 3 blocks size takes 3x3 grid of pixels, finds average color value, puts 1 char
//adjust the block size aka resolution on line 48
//pass the file path of any image on your computer

//fit to terminal width
int getTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}


int main(int argc, char** argv) {
    //upload the image
    if(argc < 2) {
        cerr << "Usage: ./ascii <image_path>\n";
        return 1;
    }
    cv::Mat image = cv::imread(argv[1]); 
    if(image.empty()) {
        cerr << "Failed to load image\n"; 
        return 1;
    }
    cout << "Image loaded successfully!\n";  

    //converting the image to black and white
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    //color value -> char
    string asciiChars = " .:-i=+*adm#%@";
    int numChars = asciiChars.size();
    int bucketSize = 256 / numChars;


    //fit to terminal 
    int termWidth = getTerminalWidth();
    int blockSize = 1;  //ADJUST FOR IMAGE RESOLUTION -->  4 = 1 char per 4x4 pixels
    double aspect = 0.5;    //compensate for the char height
    int newHeight = gray.rows * termWidth * aspect / gray.cols;
    cv::resize(gray, gray, cv::Size(termWidth, newHeight));



    //loop through each pixel
    ofstream out("output.txt"); //txt file for copy paste

    for(int y = 0; y < gray.rows; y+=blockSize) {
        for(int x = 0; x < gray.cols; x+=blockSize) {
            int sum = 0, count = 0;
            //uchar pixelValue = gray.at<uchar>(y,x); //pixel value is 0-255
            //pixels in each block
            for(int dy = 0; dy < blockSize; dy++) {
                for(int dx = 0; dx < blockSize; dx++) {
                    int ny = y + dy;
                    int nx = x + dx;
                    if(ny < gray.rows && nx < gray.cols) {
                        sum += gray.at<uchar>(y,x);
                        count++;
                    }
                }
            }
            int avg = sum / count;
            int index = avg / bucketSize;
            if(index >= numChars) index = numChars - 1;


            cout << asciiChars[index] << std::flush;
            usleep(800);   // microseconds per char
            //out << asciiChars[index]; //write to file
        }
        //next row
        cout << '\n';
        usleep(15000);  //line spacing delay
        //out << '\n';
    }
    out.close();




    return 0;

}
