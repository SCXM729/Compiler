#include "lexio.h"
#include "../toolcpp/hash.hpp"
#include <format>
#include <iostream>
#include <sstream>

#ifdef DEBUG
#define D(x) x
#else
#define D(x)
#endif

InOut::InOut(std::string_view inputFile, std::string_view outputFile)
    : Input_file_name{inputFile}, Output_file_name{outputFile},
      in{inputFile.data()}, out{outputFile.data()} {
  if (!in.is_open()) {
    std::cout << "doesn't open inputfile" << std::endl;
    exit(1);
  }
  if (!out.is_open()) {
    std::cout << "doesn't open outputfile" << std::endl;
    exit(1);
  }
}
/* copy constructor */
InOut::InOut(const InOut &src)
    : Input_file_name{src.Input_file_name},
      Output_file_name{src.Output_file_name}, in{Input_file_name.data()},
      out{Output_file_name.data()}, Actual_lineno{src.Actual_lineno},
      Lineno{src.Lineno}, cmd{src.cmd}, macros{src.macros}, RAs{src.RAs} {
  std::cout << "call InOut copy constructor" << std::endl;
  memcpy(Input_buf, src.Input_buf, 2048);
}
void InOut::swap(InOut &other) noexcept {
  std::swap(Input_file_name, other.Input_file_name);
  std::swap(Output_file_name, other.Output_file_name);
  std::swap(Actual_lineno, other.Actual_lineno);
  std::swap(Lineno, other.Lineno);
  std::swap(cmd, other.cmd);
  std::swap(macros, other.macros);
  std::swap(RAs, other.RAs);
}

/* assignment operator */
InOut &InOut::operator=(const InOut &rhs) {
  InOut tmp{rhs};
  swap(tmp);
  return *this;
}
void InOut::cleanup() noexcept {}
void InOut::moveFrom(InOut &src) noexcept {
  Input_file_name = std::exchange(src.Input_file_name, "");
  Output_file_name = std::exchange(src.Output_file_name, "");
  std::swap(Input_buf, src.Input_buf);
  memset(src.Input_buf, 0, sizeof(Input_buf));

  Actual_lineno = std::exchange(src.Actual_lineno, 0);
  Lineno = std::exchange(src.Lineno, 0);

  cmd = std::move(src.cmd);
  macros = std::move(macros);
  RAs = std::move(RAs);
}
InOut::InOut(InOut &&src) noexcept {
  std::cout << "call move constructor" << std::endl;
  moveFrom(src);
}
InOut &InOut::operator=(InOut &&rhs) noexcept {
  if (this == &rhs)
    return *this;
  cleanup();
  moveFrom(rhs);
  return *this;
}
InOut::~InOut() {} /* the files automatically close */

/*----------------------------------------------------------------------*/

/* scan through the string, replacing C-like comments with space characters.
 * Multiple-line comments are supported.*/
static void strip_comments(char *str) {
  static int incomment = 0;

  for (; *str; ++str) {
    if (incomment) {
      if (str[0] == '*' && str[1] == '/') {
        incomment = 0;
        *str++ = ' ';
      }

      if (!isspace(*str))
        *str = ' ';
    } else {
      /* not in comment  */
      if (str[0] == '/' && str[1] == '*') {
        incomment = 1;
        *str++ = ' ';
        *str = ' ';
      }
    }
  }
}

/* output all code block to outputfile before any macro definitions */
void InOut::getCodeBlocks() {
  bool transparent{false}; /* true if in a %{ .. %} code block */

  while (in.getline(Input_buf, 2048)) {
    ++Actual_lineno;

    /* if not in %{ ... %} strip comments */
    if (!transparent)
      strip_comments(Input_buf);

    if (cmd.Verbose > 1) {
      std::cout << std::format("h{}: {}\n", Actual_lineno, Input_buf);
    }

    if (Input_buf[0] == '%') {
      if (Input_buf[1] == '{') {
        transparent = !transparent;
        Actual_lineno++;
        continue;
      } else if (Input_buf[1] == '}') {
        break;
      }
    }
    /* if in %{ .. %} */
    if (transparent) {
      out << Input_buf;
      out << '\n';
    }
  }
}

void InOut::getAllMacros() {
  while (in.getline(Input_buf, 2048)) {

    if (Input_buf[0] == '%')
      if (Input_buf[1] == '%') {
        Actual_lineno++;
        break;
      }

    /* strip the comment in macro definitions */
    strip_comments(Input_buf);

    ++Actual_lineno;

    if (cmd.Verbose > 1) {
      std::cout << std::format("h{}: {}\n", Actual_lineno, Input_buf);
    }

    /* if Input_buf is a blank line */
    if (strlen(Input_buf) == 0)
      continue;

    char *def = Input_buf;
    char *name;      /* name component */
    char *text;      /* definition */
    char *endOfText; /* end of definition */

    /* isolate name */
    for (name = def; *def && !std::isspace(*def); def++)
      ;

    if (*def)
      *def++ = '\0';

    while (std::isspace(*def)) /* skip up to macro body */
      ++def;

    text = def; /* remember start of replacement text */

    endOfText = nullptr;
    while (*def) {
      if (!isspace(*def))
        ++def;
      else
        for (endOfText = def++; std::isspace(*def); ++def)
          ;
    }

    if (endOfText)
      *endOfText = '\0';

    Macro p;
    p.name = std::string(name);
    p.text = std::string(text);

    macros.addsym(p);
  }
}

static void print_a_macro(Macro *mac) {
  std::cout << std::format("--- {} --- [{}]---", mac->name, mac->text)
            << std::endl;
}
void InOut::printMacros() { macros.ptab(print_a_macro); }

void InOut::testMacros() {
  std::string name = "o";
  Macro mac{.name = name};
  auto p = macros.findsym(mac);
  if (!p.has_value()) {
    std::cout << "not found" << std::endl;
    exit(1);
  }
  std::cout << p.value()->text << std::endl;
}

/* gets all regular expression and the associated string from the input
 * stream. Stop if a line beginning with %% is encountered. All blank line are
 * discarded and all lines that start with whitespace are concatenated to the
 * previous line. Lineno is set to the line number of the top line of a
 * Multiple-line block. Actual_lineno holds the real line number.
 */
void InOut::getAllExprs() {

  Lineno = Actual_lineno;
  std::ostringstream rule;
  std::ostringstream action;
  // std::string rule;
  // std::string action;

  /* used for multi rule single action indicated by vertical bar */
  std::ostringstream multiaction;

  std::vector<std::tuple<std::string, std::string, bool>> rules{};
  rules.resize(60);

  int ruleid = -1;
  while (in.getline(Input_buf, 2048)) {

    if (cmd.Verbose > 1)
      std::cout << std::format("b{}: {}", Actual_lineno, Input_buf)
                << std::endl;

    /* skip blank line */
    if (strlen(Input_buf) == 0) {
      Actual_lineno++;
      continue;
    }

    Actual_lineno++;
    char *p = Input_buf;

    /* if not encounter space */
    if (!isspace(*p)) {

      if (ruleid != -1) {
        std::get<0>(rules.at(ruleid)) = rule.str();
        // rule.str("");
        rule.str("");
        if (std::get<2>(rules.at(ruleid)) == true) {
          std::get<1>(rules.at(ruleid)) = action.str();
          action.str("");
        }
      }

      int i = -1;
      ruleid++;

      /* get regular expression */
      while (!isspace(*p)) {
        rule << *p;
        p++;
      }

      /* skip the ws between regular expression and action */
      while (isspace(*p))
        ++p;

      if (*p == '|') {
        /* if the action is '|' signed it with false */
        std::get<2>(rules.at(ruleid)) = false;
      } else {
        std::get<2>(rules.at(ruleid)) = true;
        while (*p) {
          action << *p;
          p++;
        }
        action << '\n';
      }
    }

    /* if the line start with space, then is the action with previous rule */
    if (isspace(Input_buf[0])) {
      char *p = Input_buf;

      while (*p) {
        action << *p;
        p++;
      }
      action << '\n';
    }

    /* if the line start with '%' break */
    if (Input_buf[0] == '%')
      break;
  }

  /* replace the | action */
  std::string tmpAction;
  for (auto iter = rules.rbegin(); iter != rules.rend(); iter++) {

    if (std::get<2>(*iter) == true) {
      tmpAction = std::get<1>(*iter);
      continue;
    } else if (std::get<2>(*iter) == false && std::get<0>(*iter).size() != 0) {
      std::get<1>(*iter) = tmpAction;
      std::get<2>(*iter) = true;
    }
  }

  for (auto iter = rules.begin(); iter != rules.end(); iter++) {
    if (std::get<1>(*iter).size() != 0)
      RAs.push_back(std::make_pair(std::get<0>(*iter), std::get<1>(*iter)));
  }
}

/* print all regular expression and associated string */
void InOut::printRAs() {
  for (auto iter = RAs.begin(); iter != RAs.end(); iter++) {
    std::cout << std::format("rule: {}   action: {}", iter->first, iter->second)
              << std::endl;
  }

  std::cout << std::format("The is {} nfa", RAs.size()) << std::endl;
}

/* the original regular expression have macros. replace the macro part with
 * text and check the legality of regular expression partially at the same
 * time */
void InOut::replaceRegWithMacros(size_t id) {

  bool inquote = false; /* processing quoted string */
  const std::string &reStr =
      RAs.at(id).first;   /* to be processed regular expression */
  std::ostringstream oss; /* recive the replaced characters */

  size_t strSize = reStr.size();
  size_t posInstr = 0; /* the position in reStr */

  while (posInstr < strSize) {

    char curPos = reStr.at(posInstr);

    /* first encounter '"', set inquote as reverse */
    if (curPos == '"') {
      inquote = !inquote;
    }

    /* while not in quoted string, if encounter '\' just skip it */
    if (!inquote && reStr.at(posInstr) == '\\') {
      oss << reStr.substr(posInstr, 2);
      posInstr += 2;
      continue;
    }

    /* find a macro */
    if (!inquote && reStr.at(posInstr) == '{') {
      posInstr++; /* skip '{' */

      /* the offet of '{' parentheses relative to '}' parentheses */
      auto r1 = reStr.substr(posInstr).find('}');
      if (r1 == std::string::npos) {
        throw "the macro doesn't have corresponding curly braces";
      } else {
        /* generate the macro to be searched */
        Macro m{.name = reStr.substr(posInstr, r1)};
        auto r = macros.findsym(m);
        if (!r.has_value()) {
          std::cout << std::format("could find the macro: {} \n", m.name);
          throw "could find the macro";
        } else
          oss << r.value()->text;

        posInstr += r1; /* posInstr move to '}' */
      }

      posInstr++;
      continue;
    }

    oss << curPos;
    posInstr++;
  }
  if (inquote == true)
    throw "the regular is wrong that have a unquoted string ";

  RAs.at(id).first = oss.str();
  // if (cmd.Verbose > 1)
  //   std::cout << std::format("original is {0}, the output string is {1}",
  //   reStr,
  //                            oss.str())
  //             << std::endl;
}

/* just traverse all the RAs and replace regular expression with macros */
void InOut::replaceAllRegWithMacros() {
  for (size_t i{0}; i < RAs.size(); i++) {
    replaceRegWithMacros(i);
  }
}
