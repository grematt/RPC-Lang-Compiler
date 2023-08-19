#include <cstdio>
#include <string>
#include <iostream>

extern "C" void print_ch8(char ch) {
  printf("%c", ch);
}

extern "C" void print_i32(int i) {
  printf("%i", i);
}

extern "C" void print_f64(double d) {
  printf("%f", d);
}

extern "C" char read_ch8() {
  return getchar();
}

extern "C" int read_i32() {
  std::string num_val;
  while (int ch {getchar()}) {
    if (isspace(ch))
      break;

    if (!isdigit(ch)) {
      printf("Invalid integer input\n");
      std::exit(1);
    }
    num_val += ch;
  }
  return std::stoi(num_val);
}

extern "C" double read_f64() {
  std::string num_val;
  int dot_count {};
  while (int ch {getchar()}) {
    if (isspace(ch))
      break;

    if (!isdigit(ch) || ch == '.') {
      printf("Invalid double input\n");
      std::exit(1);
    }

    if (ch == '.')
      ++dot_count;

    num_val += ch;
  }
  
  if (dot_count > 1) {
    printf("Invalid double input\n");
    std::exit(1);
  }
  return std::stod(num_val);
}

extern "C" char read_string_by_char() {
  static int index {};
  static bool been_read{false};
  static std::string str;

  if (!been_read) {
    std::getline(std::cin, str);
    been_read = true;
  }

  char ret {str[index++]};
  if (ret == '\0')
    been_read = false;
  
  return ret;
}