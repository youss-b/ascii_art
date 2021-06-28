#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <Magick++.h>

using namespace std;
using namespace Magick;

const string kAsciiMap = "`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

void Debug(const Color& color) {
  cout << "(" << color.quantumRed() << ", " << color.quantumGreen() << ", " << color.quantumBlue() << ", " << color.quantumAlpha() << ")\n";
}

void Debug(const vector<vector<Color>>& quant_vec) {
  cout << "Iterating through pixels: \n";
  for (const auto& row : quant_vec) {
    for (const auto& color : row) {
      Debug(color);
    }
  }
}

template <typename T>
void Debug(const vector<vector<T>>& vals) {
  cout << "Iterating through: \n";
  for (const auto& row : vals) {
    for (const auto val : row) {
      cout << val << endl;
    }
  }
}

int Average(float a, float b, float c) {
  return 255 * (a + b + c) / 3;
}

int MapToBrightness(const Image& img, int row, int col) {
  ColorRGB  color (img.pixelColor(row, col));
  return Average(color.red(), color.green(), color.blue());
}

char MapToChar(int brightness) {
  int index = floor(brightness / static_cast<double>(255) * (kAsciiMap.size()-1));
  return kAsciiMap[index];
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cout << "Not enough arguments! At least need an image filepath...\n";
    return -1;
  }
  Image image(argv[1]);
  image.scale("10%x10%");
  bool debug_enabled = false;
  if (argc >= 3) {
    string debug_str = argv[2];
    debug_enabled = debug_str == "debug";
  }

  int width = image.columns();
  int height = image.rows();
  if (debug_enabled) {
    cout << "Image width: " << width << endl;
    cout << "Image height: " << height << endl << endl;
  }

  vector<vector<char>> chars(height, vector<char>(width));
  for (int row = 0; row < height; ++row) {
    for (int col = 0; col < width; ++col) {
      chars[row][col] = MapToChar(MapToBrightness(image, col, row));
      cout << chars[row][col] << chars[row][col] << chars[row][col];
    }
    cout << endl;
  }
  if (debug_enabled) Debug(chars);
  if (debug_enabled) {
    cout << "Image width: " << width << endl;
    cout << "Image height: " << height << endl << endl;
  }
  cout << "Complete! Exiting...\n";
  return 0;
}
