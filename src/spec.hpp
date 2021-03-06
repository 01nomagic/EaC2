#ifndef EAC2_SPEC_HPP
#define EAC2_SPEC_HPP

#include <string>
#include <map>
#include <vector>
#include "utils.hpp"

using namespace std;

namespace Spec {
  enum class SymbolType {
    NON_TERMINAL_SYMBOL,
    TERMINAL_SYMBOL,
    EPSILON,
    NONE
  };

  template<typename NonterminalSymbolType, typename TerminalSymbolType>
  class Symbol {
  public:

    using _NonterminalSymbolType = NonterminalSymbolType;
    using _TerminalSymbolType = TerminalSymbolType;
    using _SymbolType = SymbolType;

    static map<TerminalSymbolType, string> TerminalDesc;
    static map<NonterminalSymbolType, string> NonterminalDesc;

    SymbolType type;
    NonterminalSymbolType nt_type;
    TerminalSymbolType t_type;
    string lex;
    int line;
    int column;

    explicit Symbol(SymbolType type) :
        type(type),
        nt_type(NonterminalSymbolType::None),
        t_type(TerminalSymbolType::none),
        lex(""),
        line(-1),
        column(-1) {}

    explicit Symbol(NonterminalSymbolType nt_type) :
        type(SymbolType::NON_TERMINAL_SYMBOL),
        nt_type(nt_type),
        t_type(TerminalSymbolType::none),
        lex(""),
        line(-1),
        column(-1) {}

    explicit Symbol(TerminalSymbolType t_type, string lex = "", int line = -1, int column = -1) :
        type(SymbolType::TERMINAL_SYMBOL),
        t_type(t_type),
        lex(lex),
        line(line),
        column(column),
        nt_type(NonterminalSymbolType::None) {}

    bool is(NonterminalSymbolType nt_type1) const {
      return type == SymbolType::NON_TERMINAL_SYMBOL && this->nt_type == nt_type1;
    }

    bool is(TerminalSymbolType t_type1, string lex1 = "") const {
      return type == SymbolType::TERMINAL_SYMBOL && this->t_type == t_type1 && this->lex == lex1;
    }

    bool isValid() const {
      return type != SymbolType::NONE;
    }

    bool isNonterminalSymbol() const {
      return type == SymbolType::NON_TERMINAL_SYMBOL && nt_type != NonterminalSymbolType::None;
    }

    bool isTerminalSymbol() const {
      return type == SymbolType::TERMINAL_SYMBOL && t_type != TerminalSymbolType::none;
    }

    bool isEpsilon() const {
      return type == SymbolType::EPSILON;
    }

    bool operator<(const Symbol &rhs) const {
      return type < rhs.type ||
             (type == rhs.type && nt_type < rhs.nt_type) ||
             (type == rhs.type && nt_type == rhs.nt_type && t_type < rhs.t_type) ||
             (type == rhs.type && nt_type == rhs.nt_type && t_type == rhs.t_type && lex < rhs.lex);
    }

    bool operator==(const Symbol &rhs) const {
      return type == rhs.type && nt_type == rhs.nt_type && t_type == rhs.t_type && lex == rhs.lex;
    }

    Symbol getPureSymbol ();

    string getDesc(bool needPos = false) {
      string pos;
      switch (type) {
        case SymbolType::NON_TERMINAL_SYMBOL:
          return NonterminalDesc[nt_type];
        case SymbolType::TERMINAL_SYMBOL:
          if (needPos) {
            pos = to_string(line) + ":" + to_string(column);
            return TerminalDesc[t_type] + (lex.size() > 0 ? (" " + escape(lex) + " " + pos) : "");
          } else {
            return TerminalDesc[t_type] + (lex.size() > 0 ? (" " + escape(lex)) : "");
          }
        default:
          return "Error";
      }
    }
  };

  template<typename TerminalSymbolType>
  using Lex = vector<pair<string, TerminalSymbolType>>;

  template<typename Symbol>
  using Production = vector<Symbol>;

  template<typename Symbol>
  using ProductionList = vector<Production<Symbol>>;

  template<typename Symbol>
  using CFG = map<Symbol, ProductionList<Symbol>>;



  template<typename Symbol>
  class AST {
  public:
    class Node {
    public:
      Symbol p;
      vector<Node> children;

      Node(Symbol s) : p(s), children() {}

      void addChild(Node c) {
        children.insert(children.begin(), c);
      }
    };

    class NodeString {
    public:
      string p;
      vector<NodeString> children;

      NodeString(Node s) : p(s.p.getDesc()), children() {
        for (auto& child : s.children) {
          if (child.p.type == SymbolType::NON_TERMINAL_SYMBOL && child.children.size() == 0) {
          } else {
            children.push_back(NodeString(child));
          }
        }
      }
    };

    Node root;
    NodeString rootString;

    AST (Node root) : root(root), rootString(root) {}

    string toString () {
      return toString(rootString, 0);
    }

    string toString (NodeString node, int tab) {
      string res = "";
      res += string(tab, ' ') + node.p + "\n";
      for (auto& item : node.children) {
        res += toString(item, tab + 2);
      }
      return res;
    }
  };

}

#endif
