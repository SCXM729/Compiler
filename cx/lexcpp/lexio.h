#pragma once
#include "../toolcpp/hash.hpp"
#include <cstring>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

/* control cmd switch value */
struct Cmd {
  int Verbose = 2;
};

struct Macro {
  std::string name;
  std::string text;
};

class InOut {
public:
  InOut(std::string_view inputFile, std::string_view outputFile);
  InOut(const InOut &src);

  void swap(InOut &other) noexcept;
  InOut &operator=(const InOut &rhs);

  InOut(InOut &&src) noexcept;
  InOut &operator=(InOut &&rhs) noexcept;

  ~InOut();

private:
  void cleanup() noexcept;
  void moveFrom(InOut &src) noexcept;
  /* handle the comments and following code blocks and macro definitions */
public:
  void getAllMacros();
  void getCodeBlocks();
  void newMacro(char *def);
  void printMacros();
  void testMacros();

  /* handle all regular expression and associated string */
  void getAllExprs();
  void printRAs();
  void replaceRegWithMacros(size_t id);
  void replaceAllRegWithMacros();

  /* passed RAs to NfaStates */
  const std::vector<std::pair<std::string, std::string>> &getRAs() const {
    return RAs;
  }

  const Cmd &getCmd() const { return cmd; }
  int getActualLineno() const { return Actual_lineno; }
  int getLineno() const { return Lineno; }

private:
  std::string_view Input_file_name;
  std::string_view Output_file_name;
  char Input_buf[2048]{};
  std::ifstream in;
  std::ofstream out;
  int Actual_lineno{};
  int Lineno{};
  Cmd cmd;
  Hash<Macro> macros{12, hash_add, simpleCmp};
  /* regular expressions and associated accepting strings created by
   * getAllExprs()*/
  std::vector<std::pair<std::string, std::string>> RAs;
};
