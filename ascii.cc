#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <Magick++.h>

using namespace std;
using namespace Magick;

const string kAsciiMap = "`^\",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

enum BRIGHT_MAP_TYPE { AVERAGE, MIN_MAX, LUMINOSITY};

struct Flags {
  bool debug_enabled = false;
  bool invert = false;
  string img_file = "";
  BRIGHT_MAP_TYPE map_type = BRIGHT_MAP_TYPE::AVERAGE;
  int unsquash_factor = 3;  // TODO: add a way to change this
};

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

int MinMax(float a, float b, float c) {
  return 255 * (max(max(a, b), c) + min(min(a, b), c)) / 2;
}

int Luminosity(double a, double b, double c) {
  return 255 * (0.21 * a + 0.72 * b + 0.07 * c);
}

int MapToBrightness(const Image& img, int row, int col, const Flags& flags) {
  ColorRGB  color (img.pixelColor(row, col));
  int avg = 0;
  switch (flags.map_type) {
    case BRIGHT_MAP_TYPE::MIN_MAX:
      avg = MinMax(color.red(), color.green(), color.blue());
      break;
    case BRIGHT_MAP_TYPE::LUMINOSITY:
      avg = Luminosity(color.red(), color.green(), color.blue());
      break;
    default:
      avg = Average(color.red(), color.green(), color.blue());
      break;
  }
  if (flags.invert) {
    avg = 128 + (128 - avg) - 1;
  }
  return avg;
}

char MapToChar(int brightness) {
  int index = floor(brightness / static_cast<double>(255) * (kAsciiMap.size()-1));
  return kAsciiMap[index];
}

int main(int argc, char** argv) {
  Flags flags;
  if (argc < 2) {
    cout << "Not enough arguments! At least need an image filepath...\n";
    return -1;
  }
  flags.img_file = argv[1];
  for (int i = 2; i < argc; ++i) {
    string str = argv[i];
    if (string(argv[i]) == "debug") {
      flags.debug_enabled = true;
    } else if (string(argv[i]) == "invert") {
      flags.invert = true;
    } else if (str == "min_max") {
      flags.map_type = BRIGHT_MAP_TYPE::MIN_MAX;
    } else if (str == "lum") {
      flags.map_type = BRIGHT_MAP_TYPE::LUMINOSITY;
    }
  }

  Image image(flags.img_file);
  image.scale("10%x10%");
  bool debug_enabled = flags.debug_enabled;
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
      chars[row][col] = MapToChar(MapToBrightness(image, col, row, flags));
      for (int i = 0; i < flags.unsquash_factor; ++i) cout << chars[row][col];
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
