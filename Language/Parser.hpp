
#include <cstring>
#include <string>
#include <iostream>
#include <stack>
#include <optional>
#include <cassert>
#include <cstdlib>

#include "Node.hpp"

using namespace std;

class Parser {
private:
  const char *input;
  stack<const char *> marks;
  stack<optional<Node *>> results;

public:
  Parser(const char *input) : input(input) {}

  inline void skip() {
    while (*input && isspace(*input))
      input++;
  }

  inline optional<char> symbol(const char *c, size_t n) {
    skip();
    if (*input && strchr(c, *input) != NULL) {
      char retval = *input;
      input++;
      return retval;
    }
    return nullopt;
  }

  inline void mark() { marks.push(input); }

  inline void unmark() { marks.pop(); }

  inline void reset() {
    input = marks.top();
    marks.pop();
  }

  inline optional<Node *> pop() {
    auto retval = results.top();
    results.pop();
    return retval;
  }

  inline void push(optional<Node *> res) { results.push(res); }

  inline void drop() {
    optional<Node *> temp = results.top();
    results.pop();
    results.top() = temp;
  }

  inline optional<Node *> &top() { return results.top(); }

  const char *pos() { return input; }

  optional<Node *> expect(const char *s, int n) {
    skip();
    if (strncmp(input, s, n) == 0) {
      input += n;
      return new Node{s};
    }
    return nullopt;
  }

  optional<Node *> parsePROGRAM() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Wildcard(op='*', alts=[[Token(type=1, str='FUNCTION')]])]]
    push(nullopt);

    // ALT [Wildcard(op='*', alts=[[Token(type=1, str='FUNCTION')]])]
    while (!top()) {
      mark();

      top() = new Node{};

      // WILDCARD * : [[Token(type=1, str='FUNCTION')]]

      while (1) {

        // ALTS [[Token(type=1, str='FUNCTION')]]
        push(nullopt);

        // ALT [Token(type=1, str='FUNCTION')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = parseFUNCTION();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=1, str='FUNCTION')]

        // END ALTS [[Token(type=1, str='FUNCTION')]]
        assert(results.size() == init_size + 2);

        tmp = pop();
        if (tmp) {

          (*top())->childs.insert((*top())->childs.end(),
                                  (*tmp)->childs.begin(), (*tmp)->childs.end());
          delete *tmp;

        } else
          break;
      }

      // END WILDCARD * : [[Token(type=1, str='FUNCTION')]]

      unmark();
    }
    // END ALT [Wildcard(op='*', alts=[[Token(type=1, str='FUNCTION')]])]

    // END ALTS [[Wildcard(op='*', alts=[[Token(type=1, str='FUNCTION')]])]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "PROGRAM";
    }

    return tmp;
  }

  optional<Node *> parseFUNCTION() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=3, str='"def"'), Token(type=1, str='ID'), Token(type=3,
    // str='"("'), Wildcard(op='?', alts=[[Token(type=1, str='IDLIST')]]),
    // Token(type=3, str='")"'), Wildcard(op='?', alts=[[Token(type=3,
    // str='":"'), Token(type=1, str='IDLIST')]]), Token(type=1, str='BLOCK')]]
    push(nullopt);

    // ALT [Token(type=3, str='"def"'), Token(type=1, str='ID'), Token(type=3,
    // str='"("'), Wildcard(op='?', alts=[[Token(type=1, str='IDLIST')]]),
    // Token(type=3, str='")"'), Wildcard(op='?', alts=[[Token(type=3,
    // str='":"'), Token(type=1, str='IDLIST')]]), Token(type=1, str='BLOCK')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = expect("def", 3);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = parseID();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect("(", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD ? : [[Token(type=1, str='IDLIST')]]

      // ALTS [[Token(type=1, str='IDLIST')]]
      push(nullopt);

      // ALT [Token(type=1, str='IDLIST')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = parseIDLIST();

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=1, str='IDLIST')]

      // END ALTS [[Token(type=1, str='IDLIST')]]
      assert(results.size() == init_size + 2);

      tmp = pop();
      if (tmp) {

        (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                                (*tmp)->childs.end());
        delete *tmp;
      }

      // END WILDCARD ? : [[Token(type=1, str='IDLIST')]]

      tmp = expect(")", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD ? : [[Token(type=3, str='":"'), Token(type=1, str='IDLIST')]]

      // ALTS [[Token(type=3, str='":"'), Token(type=1, str='IDLIST')]]
      push(nullopt);

      // ALT [Token(type=3, str='":"'), Token(type=1, str='IDLIST')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = expect(":", 1);

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        tmp = parseIDLIST();

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=3, str='":"'), Token(type=1, str='IDLIST')]

      // END ALTS [[Token(type=3, str='":"'), Token(type=1, str='IDLIST')]]
      assert(results.size() == init_size + 2);

      tmp = pop();
      if (tmp) {

        (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                                (*tmp)->childs.end());
        delete *tmp;
      }

      // END WILDCARD ? : [[Token(type=3, str='":"'), Token(type=1,
      // str='IDLIST')]]

      tmp = parseBLOCK();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=3, str='"def"'), Token(type=1, str='ID'),
    // Token(type=3, str='"("'), Wildcard(op='?', alts=[[Token(type=1,
    // str='IDLIST')]]), Token(type=3, str='")"'), Wildcard(op='?',
    // alts=[[Token(type=3, str='":"'), Token(type=1, str='IDLIST')]]),
    // Token(type=1, str='BLOCK')]

    // END ALTS [[Token(type=3, str='"def"'), Token(type=1, str='ID'),
    // Token(type=3, str='"("'), Wildcard(op='?', alts=[[Token(type=1,
    // str='IDLIST')]]), Token(type=3, str='")"'), Wildcard(op='?',
    // alts=[[Token(type=3, str='":"'), Token(type=1, str='IDLIST')]]),
    // Token(type=1, str='BLOCK')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "FUNCTION";
    }

    return tmp;
  }

  optional<Node *> parseBLOCK() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=3, str='"{"'), Wildcard(op='*', alts=[[Token(type=1,
    // str='CALL')], [Token(type=1, str='STATEMENT')], [Token(type=1,
    // str='IFSTATEMENT')], [Token(type=1, str='RETSTATEMENT')]]), Token(type=3,
    // str='"}"')]]
    push(nullopt);

    // ALT [Token(type=3, str='"{"'), Wildcard(op='*', alts=[[Token(type=1,
    // str='CALL')], [Token(type=1, str='STATEMENT')], [Token(type=1,
    // str='IFSTATEMENT')], [Token(type=1, str='RETSTATEMENT')]]), Token(type=3,
    // str='"}"')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = expect("{", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD * : [[Token(type=1, str='CALL')], [Token(type=1,
      // str='STATEMENT')], [Token(type=1, str='IFSTATEMENT')], [Token(type=1,
      // str='RETSTATEMENT')]]

      while (1) {

        // ALTS [[Token(type=1, str='CALL')], [Token(type=1, str='STATEMENT')],
        // [Token(type=1, str='IFSTATEMENT')], [Token(type=1,
        // str='RETSTATEMENT')]]
        push(nullopt);

        // ALT [Token(type=1, str='CALL')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = parseCALL();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=1, str='CALL')]

        // ALT [Token(type=1, str='STATEMENT')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = parseSTATEMENT();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=1, str='STATEMENT')]

        // ALT [Token(type=1, str='IFSTATEMENT')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = parseIFSTATEMENT();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=1, str='IFSTATEMENT')]

        // ALT [Token(type=1, str='RETSTATEMENT')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = parseRETSTATEMENT();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=1, str='RETSTATEMENT')]

        // END ALTS [[Token(type=1, str='CALL')], [Token(type=1,
        // str='STATEMENT')], [Token(type=1, str='IFSTATEMENT')], [Token(type=1,
        // str='RETSTATEMENT')]]
        assert(results.size() == init_size + 2);

        tmp = pop();
        if (tmp) {

          (*top())->childs.insert((*top())->childs.end(),
                                  (*tmp)->childs.begin(), (*tmp)->childs.end());
          delete *tmp;

        } else
          break;
      }

      // END WILDCARD * : [[Token(type=1, str='CALL')], [Token(type=1,
      // str='STATEMENT')], [Token(type=1, str='IFSTATEMENT')], [Token(type=1,
      // str='RETSTATEMENT')]]

      tmp = expect("}", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=3, str='"{"'), Wildcard(op='*', alts=[[Token(type=1,
    // str='CALL')], [Token(type=1, str='STATEMENT')], [Token(type=1,
    // str='IFSTATEMENT')], [Token(type=1, str='RETSTATEMENT')]]), Token(type=3,
    // str='"}"')]

    // END ALTS [[Token(type=3, str='"{"'), Wildcard(op='*',
    // alts=[[Token(type=1, str='CALL')], [Token(type=1, str='STATEMENT')],
    // [Token(type=1, str='IFSTATEMENT')], [Token(type=1,
    // str='RETSTATEMENT')]]), Token(type=3, str='"}"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "BLOCK";
    }

    return tmp;
  }

  optional<Node *> parseIDLIST() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='ID'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='ID')]])]]
    push(nullopt);

    // ALT [Token(type=1, str='ID'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='ID')]])]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseID();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD * : [[Token(type=3, str='","'), Token(type=1, str='ID')]]

      while (1) {

        // ALTS [[Token(type=3, str='","'), Token(type=1, str='ID')]]
        push(nullopt);

        // ALT [Token(type=3, str='","'), Token(type=1, str='ID')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = expect(",", 1);

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          tmp = parseID();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=3, str='","'), Token(type=1, str='ID')]

        // END ALTS [[Token(type=3, str='","'), Token(type=1, str='ID')]]
        assert(results.size() == init_size + 2);

        tmp = pop();
        if (tmp) {

          (*top())->childs.insert((*top())->childs.end(),
                                  (*tmp)->childs.begin(), (*tmp)->childs.end());
          delete *tmp;

        } else
          break;
      }

      // END WILDCARD * : [[Token(type=3, str='","'), Token(type=1, str='ID')]]

      unmark();
    }
    // END ALT [Token(type=1, str='ID'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='ID')]])]

    // END ALTS [[Token(type=1, str='ID'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='ID')]])]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "IDLIST";
    }

    return tmp;
  }

  optional<Node *> parseEXPR() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='T'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"+-"'), Token(type=1, str='T')]])]]
    push(nullopt);

    // ALT [Token(type=1, str='T'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"+-"'), Token(type=1, str='T')]])]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseT();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD * : [[Charset(op='', chars='"+-"'), Token(type=1, str='T')]]

      while (1) {

        // ALTS [[Charset(op='', chars='"+-"'), Token(type=1, str='T')]]
        push(nullopt);

        // ALT [Charset(op='', chars='"+-"'), Token(type=1, str='T')]
        while (!top()) {
          mark();

          top() = new Node{};

          ctmp = symbol("+-", 2);

          if (!ctmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(new Node{string(1, *ctmp)});

          tmp = parseT();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Charset(op='', chars='"+-"'), Token(type=1, str='T')]

        // END ALTS [[Charset(op='', chars='"+-"'), Token(type=1, str='T')]]
        assert(results.size() == init_size + 2);

        tmp = pop();
        if (tmp) {

          (*top())->childs.insert((*top())->childs.end(),
                                  (*tmp)->childs.begin(), (*tmp)->childs.end());
          delete *tmp;

        } else
          break;
      }

      // END WILDCARD * : [[Charset(op='', chars='"+-"'), Token(type=1,
      // str='T')]]

      unmark();
    }
    // END ALT [Token(type=1, str='T'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"+-"'), Token(type=1, str='T')]])]

    // END ALTS [[Token(type=1, str='T'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"+-"'), Token(type=1, str='T')]])]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "EXPR";
    }

    return tmp;
  }

  optional<Node *> parseT() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='A'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"*/"'), Token(type=1, str='A')]])]]
    push(nullopt);

    // ALT [Token(type=1, str='A'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"*/"'), Token(type=1, str='A')]])]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseA();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD * : [[Charset(op='', chars='"*/"'), Token(type=1, str='A')]]

      while (1) {

        // ALTS [[Charset(op='', chars='"*/"'), Token(type=1, str='A')]]
        push(nullopt);

        // ALT [Charset(op='', chars='"*/"'), Token(type=1, str='A')]
        while (!top()) {
          mark();

          top() = new Node{};

          ctmp = symbol("*/", 2);

          if (!ctmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(new Node{string(1, *ctmp)});

          tmp = parseA();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Charset(op='', chars='"*/"'), Token(type=1, str='A')]

        // END ALTS [[Charset(op='', chars='"*/"'), Token(type=1, str='A')]]
        assert(results.size() == init_size + 2);

        tmp = pop();
        if (tmp) {

          (*top())->childs.insert((*top())->childs.end(),
                                  (*tmp)->childs.begin(), (*tmp)->childs.end());
          delete *tmp;

        } else
          break;
      }

      // END WILDCARD * : [[Charset(op='', chars='"*/"'), Token(type=1,
      // str='A')]]

      unmark();
    }
    // END ALT [Token(type=1, str='A'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"*/"'), Token(type=1, str='A')]])]

    // END ALTS [[Token(type=1, str='A'), Wildcard(op='*', alts=[[Charset(op='',
    // chars='"*/"'), Token(type=1, str='A')]])]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "T";
    }

    return tmp;
  }

  optional<Node *> parseA() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='NUM')], [Charset(op='?', chars='"+-"'),
    // Token(type=1, str='ID')], [Token(type=3, str='"("'), Token(type=1,
    // str='EXPR'), Token(type=3, str='")"')]]
    push(nullopt);

    // ALT [Token(type=1, str='NUM')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseNUM();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=1, str='NUM')]

    // ALT [Charset(op='?', chars='"+-"'), Token(type=1, str='ID')]
    while (!top()) {
      mark();

      top() = new Node{};

      ctmp = symbol("+-", 2);

      if (ctmp) {

        (*top())->childs.push_back(new Node{string(1, *ctmp)});
      }

      tmp = parseID();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Charset(op='?', chars='"+-"'), Token(type=1, str='ID')]

    // ALT [Token(type=3, str='"("'), Token(type=1, str='EXPR'), Token(type=3,
    // str='")"')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = expect("(", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = parseEXPR();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect(")", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=3, str='"("'), Token(type=1, str='EXPR'),
    // Token(type=3, str='")"')]

    // END ALTS [[Token(type=1, str='NUM')], [Charset(op='?', chars='"+-"'),
    // Token(type=1, str='ID')], [Token(type=3, str='"("'), Token(type=1,
    // str='EXPR'), Token(type=3, str='")"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "A";
    }

    return tmp;
  }

  optional<Node *> parseNUM() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Charset(op='?', chars='"+-"'), Charset(op='',
    // chars='"123456789"'), Charset(op='*', chars='"1234567890"')],
    // [Token(type=3, str='"0"')]]
    push(nullopt);

    // ALT [Charset(op='?', chars='"+-"'), Charset(op='', chars='"123456789"'),
    // Charset(op='*', chars='"1234567890"')]
    while (!top()) {
      mark();

      top() = new Node{};

      ctmp = symbol("+-", 2);

      if (ctmp) {

        (*top())->childs.push_back(new Node{string(1, *ctmp)});
      }

      ctmp = symbol("123456789", 9);

      if (!ctmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(new Node{string(1, *ctmp)});

      ctmp = symbol("1234567890", 10);

      if (ctmp) {

        (*top())->childs.push_back(new Node{string(1, *ctmp)});
      }

      while (ctmp) {

        ctmp = symbol("1234567890", 10);

        if (ctmp) {

          (*top())->childs.back()->data += *ctmp;

        } else
          break;
      }

      unmark();
    }
    // END ALT [Charset(op='?', chars='"+-"'), Charset(op='',
    // chars='"123456789"'), Charset(op='*', chars='"1234567890"')]

    // ALT [Token(type=3, str='"0"')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = expect("0", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=3, str='"0"')]

    // END ALTS [[Charset(op='?', chars='"+-"'), Charset(op='',
    // chars='"123456789"'), Charset(op='*', chars='"1234567890"')],
    // [Token(type=3, str='"0"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "NUM";
    }

    return tmp;
  }

  optional<Node *> parseID() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Charset(op='',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"'),
    // Charset(op='*',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890"')]]
    push(nullopt);

    // ALT [Charset(op='',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"'),
    // Charset(op='*',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890"')]
    while (!top()) {
      mark();

      top() = new Node{};

      ctmp = symbol("qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM", 52);

      if (!ctmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(new Node{string(1, *ctmp)});

      ctmp = symbol(
          "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890", 62);

      if (ctmp) {

        (*top())->childs.push_back(new Node{string(1, *ctmp)});
      }

      while (ctmp) {

        ctmp = symbol(
            "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890",
            62);

        if (ctmp) {

          (*top())->childs.back()->data += *ctmp;

        } else
          break;
      }

      unmark();
    }
    // END ALT [Charset(op='',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"'),
    // Charset(op='*',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890"')]

    // END ALTS [[Charset(op='',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"'),
    // Charset(op='*',
    // chars='"qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "ID";
    }

    return tmp;
  }

  optional<Node *> parseEXPRLIST() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='EXPR'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='EXPR')]])]]
    push(nullopt);

    // ALT [Token(type=1, str='EXPR'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='EXPR')]])]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseEXPR();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD * : [[Token(type=3, str='","'), Token(type=1, str='EXPR')]]

      while (1) {

        // ALTS [[Token(type=3, str='","'), Token(type=1, str='EXPR')]]
        push(nullopt);

        // ALT [Token(type=3, str='","'), Token(type=1, str='EXPR')]
        while (!top()) {
          mark();

          top() = new Node{};

          tmp = expect(",", 1);

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          tmp = parseEXPR();

          if (!tmp) {
            purge_tree(*top());
            top() = nullopt;

            reset();
            break;
          }

          (*top())->childs.push_back(*tmp);

          unmark();
        }
        // END ALT [Token(type=3, str='","'), Token(type=1, str='EXPR')]

        // END ALTS [[Token(type=3, str='","'), Token(type=1, str='EXPR')]]
        assert(results.size() == init_size + 2);

        tmp = pop();
        if (tmp) {

          (*top())->childs.insert((*top())->childs.end(),
                                  (*tmp)->childs.begin(), (*tmp)->childs.end());
          delete *tmp;

        } else
          break;
      }

      // END WILDCARD * : [[Token(type=3, str='","'), Token(type=1,
      // str='EXPR')]]

      unmark();
    }
    // END ALT [Token(type=1, str='EXPR'), Wildcard(op='*', alts=[[Token(type=3,
    // str='","'), Token(type=1, str='EXPR')]])]

    // END ALTS [[Token(type=1, str='EXPR'), Wildcard(op='*',
    // alts=[[Token(type=3, str='","'), Token(type=1, str='EXPR')]])]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "EXPRLIST";
    }

    return tmp;
  }

  optional<Node *> parseSTATEMENT() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='IDLIST'), Token(type=3, str='"="'),
    // Token(type=1, str='EXPRLIST'), Token(type=3, str='";"')]]
    push(nullopt);

    // ALT [Token(type=1, str='IDLIST'), Token(type=3, str='"="'), Token(type=1,
    // str='EXPRLIST'), Token(type=3, str='";"')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseIDLIST();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect("=", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = parseEXPRLIST();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect(";", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=1, str='IDLIST'), Token(type=3, str='"="'),
    // Token(type=1, str='EXPRLIST'), Token(type=3, str='";"')]

    // END ALTS [[Token(type=1, str='IDLIST'), Token(type=3, str='"="'),
    // Token(type=1, str='EXPRLIST'), Token(type=3, str='";"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "STATEMENT";
    }

    return tmp;
  }

  optional<Node *> parseIFSTATEMENT() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=3, str='"if"'), Token(type=3, str='"("'), Token(type=1,
    // str='BOOLEXPR'), Token(type=3, str='")"'), Token(type=1, str='BLOCK'),
    // Wildcard(op='?', alts=[[Token(type=3, str='"else"'), Token(type=1,
    // str='BLOCK')]])]]
    push(nullopt);

    // ALT [Token(type=3, str='"if"'), Token(type=3, str='"("'), Token(type=1,
    // str='BOOLEXPR'), Token(type=3, str='")"'), Token(type=1, str='BLOCK'),
    // Wildcard(op='?', alts=[[Token(type=3, str='"else"'), Token(type=1,
    // str='BLOCK')]])]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = expect("if", 2);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect("(", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = parseBOOLEXPR();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect(")", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = parseBLOCK();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD ? : [[Token(type=3, str='"else"'), Token(type=1,
      // str='BLOCK')]]

      // ALTS [[Token(type=3, str='"else"'), Token(type=1, str='BLOCK')]]
      push(nullopt);

      // ALT [Token(type=3, str='"else"'), Token(type=1, str='BLOCK')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = expect("else", 4);

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        tmp = parseBLOCK();

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=3, str='"else"'), Token(type=1, str='BLOCK')]

      // END ALTS [[Token(type=3, str='"else"'), Token(type=1, str='BLOCK')]]
      assert(results.size() == init_size + 2);

      tmp = pop();
      if (tmp) {

        (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                                (*tmp)->childs.end());
        delete *tmp;
      }

      // END WILDCARD ? : [[Token(type=3, str='"else"'), Token(type=1,
      // str='BLOCK')]]

      unmark();
    }
    // END ALT [Token(type=3, str='"if"'), Token(type=3, str='"("'),
    // Token(type=1, str='BOOLEXPR'), Token(type=3, str='")"'), Token(type=1,
    // str='BLOCK'), Wildcard(op='?', alts=[[Token(type=3, str='"else"'),
    // Token(type=1, str='BLOCK')]])]

    // END ALTS [[Token(type=3, str='"if"'), Token(type=3, str='"("'),
    // Token(type=1, str='BOOLEXPR'), Token(type=3, str='")"'), Token(type=1,
    // str='BLOCK'), Wildcard(op='?', alts=[[Token(type=3, str='"else"'),
    // Token(type=1, str='BLOCK')]])]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "IFSTATEMENT";
    }

    return tmp;
  }

  optional<Node *> parseRETSTATEMENT() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=3, str='"return"'), Wildcard(op='?',
    // alts=[[Token(type=1, str='EXPRLIST')]]), Token(type=3, str='";"')]]
    push(nullopt);

    // ALT [Token(type=3, str='"return"'), Wildcard(op='?', alts=[[Token(type=1,
    // str='EXPRLIST')]]), Token(type=3, str='";"')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = expect("return", 6);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD ? : [[Token(type=1, str='EXPRLIST')]]

      // ALTS [[Token(type=1, str='EXPRLIST')]]
      push(nullopt);

      // ALT [Token(type=1, str='EXPRLIST')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = parseEXPRLIST();

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=1, str='EXPRLIST')]

      // END ALTS [[Token(type=1, str='EXPRLIST')]]
      assert(results.size() == init_size + 2);

      tmp = pop();
      if (tmp) {

        (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                                (*tmp)->childs.end());
        delete *tmp;
      }

      // END WILDCARD ? : [[Token(type=1, str='EXPRLIST')]]

      tmp = expect(";", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=3, str='"return"'), Wildcard(op='?',
    // alts=[[Token(type=1, str='EXPRLIST')]]), Token(type=3, str='";"')]

    // END ALTS [[Token(type=3, str='"return"'), Wildcard(op='?',
    // alts=[[Token(type=1, str='EXPRLIST')]]), Token(type=3, str='";"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "RETSTATEMENT";
    }

    return tmp;
  }

  optional<Node *> parseBOOLEXPR() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Token(type=1, str='EXPR'), Wildcard(op='', alts=[[Token(type=3,
    // str='">="')], [Token(type=3, str='"=="')], [Token(type=3, str='">"')]]),
    // Token(type=1, str='EXPR')]]
    push(nullopt);

    // ALT [Token(type=1, str='EXPR'), Wildcard(op='', alts=[[Token(type=3,
    // str='">="')], [Token(type=3, str='"=="')], [Token(type=3, str='">"')]]),
    // Token(type=1, str='EXPR')]
    while (!top()) {
      mark();

      top() = new Node{};

      tmp = parseEXPR();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD  : [[Token(type=3, str='">="')], [Token(type=3, str='"=="')],
      // [Token(type=3, str='">"')]]

      // ALTS [[Token(type=3, str='">="')], [Token(type=3, str='"=="')],
      // [Token(type=3, str='">"')]]
      push(nullopt);

      // ALT [Token(type=3, str='">="')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = expect(">=", 2);

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=3, str='">="')]

      // ALT [Token(type=3, str='"=="')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = expect("==", 2);

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=3, str='"=="')]

      // ALT [Token(type=3, str='">"')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = expect(">", 1);

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=3, str='">"')]

      // END ALTS [[Token(type=3, str='">="')], [Token(type=3, str='"=="')],
      // [Token(type=3, str='">"')]]
      assert(results.size() == init_size + 2);

      tmp = pop();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                              (*tmp)->childs.end());
      delete *tmp;

      // END WILDCARD  : [[Token(type=3, str='">="')], [Token(type=3,
      // str='"=="')], [Token(type=3, str='">"')]]

      tmp = parseEXPR();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Token(type=1, str='EXPR'), Wildcard(op='', alts=[[Token(type=3,
    // str='">="')], [Token(type=3, str='"=="')], [Token(type=3, str='">"')]]),
    // Token(type=1, str='EXPR')]

    // END ALTS [[Token(type=1, str='EXPR'), Wildcard(op='',
    // alts=[[Token(type=3, str='">="')], [Token(type=3, str='"=="')],
    // [Token(type=3, str='">"')]]), Token(type=1, str='EXPR')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "BOOLEXPR";
    }

    return tmp;
  }

  optional<Node *> parseCALL() {
    const size_t init_size = results.size();

    optional<Node *> tmp = nullopt;
    optional<char> ctmp = nullopt;

    // ALTS [[Wildcard(op='?', alts=[[Token(type=1, str='IDLIST'), Token(type=3,
    // str='"="')]]), Token(type=1, str='ID'), Token(type=3, str='"("'),
    // Wildcard(op='?', alts=[[Token(type=1, str='EXPRLIST')]]), Token(type=3,
    // str='")"'), Token(type=3, str='";"')]]
    push(nullopt);

    // ALT [Wildcard(op='?', alts=[[Token(type=1, str='IDLIST'), Token(type=3,
    // str='"="')]]), Token(type=1, str='ID'), Token(type=3, str='"("'),
    // Wildcard(op='?', alts=[[Token(type=1, str='EXPRLIST')]]), Token(type=3,
    // str='")"'), Token(type=3, str='";"')]
    while (!top()) {
      mark();

      top() = new Node{};

      // WILDCARD ? : [[Token(type=1, str='IDLIST'), Token(type=3, str='"="')]]

      // ALTS [[Token(type=1, str='IDLIST'), Token(type=3, str='"="')]]
      push(nullopt);

      // ALT [Token(type=1, str='IDLIST'), Token(type=3, str='"="')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = parseIDLIST();

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        tmp = expect("=", 1);

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=1, str='IDLIST'), Token(type=3, str='"="')]

      // END ALTS [[Token(type=1, str='IDLIST'), Token(type=3, str='"="')]]
      assert(results.size() == init_size + 2);

      tmp = pop();
      if (tmp) {

        (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                                (*tmp)->childs.end());
        delete *tmp;
      }

      // END WILDCARD ? : [[Token(type=1, str='IDLIST'), Token(type=3,
      // str='"="')]]

      tmp = parseID();

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect("(", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      // WILDCARD ? : [[Token(type=1, str='EXPRLIST')]]

      // ALTS [[Token(type=1, str='EXPRLIST')]]
      push(nullopt);

      // ALT [Token(type=1, str='EXPRLIST')]
      while (!top()) {
        mark();

        top() = new Node{};

        tmp = parseEXPRLIST();

        if (!tmp) {
          purge_tree(*top());
          top() = nullopt;

          reset();
          break;
        }

        (*top())->childs.push_back(*tmp);

        unmark();
      }
      // END ALT [Token(type=1, str='EXPRLIST')]

      // END ALTS [[Token(type=1, str='EXPRLIST')]]
      assert(results.size() == init_size + 2);

      tmp = pop();
      if (tmp) {

        (*top())->childs.insert((*top())->childs.end(), (*tmp)->childs.begin(),
                                (*tmp)->childs.end());
        delete *tmp;
      }

      // END WILDCARD ? : [[Token(type=1, str='EXPRLIST')]]

      tmp = expect(")", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      tmp = expect(";", 1);

      if (!tmp) {
        purge_tree(*top());
        top() = nullopt;

        reset();
        break;
      }

      (*top())->childs.push_back(*tmp);

      unmark();
    }
    // END ALT [Wildcard(op='?', alts=[[Token(type=1, str='IDLIST'),
    // Token(type=3, str='"="')]]), Token(type=1, str='ID'), Token(type=3,
    // str='"("'), Wildcard(op='?', alts=[[Token(type=1, str='EXPRLIST')]]),
    // Token(type=3, str='")"'), Token(type=3, str='";"')]

    // END ALTS [[Wildcard(op='?', alts=[[Token(type=1, str='IDLIST'),
    // Token(type=3, str='"="')]]), Token(type=1, str='ID'), Token(type=3,
    // str='"("'), Wildcard(op='?', alts=[[Token(type=1, str='EXPRLIST')]]),
    // Token(type=3, str='")"'), Token(type=3, str='";"')]]
    assert(results.size() == init_size + 1);

    tmp = pop();
    if (tmp) {
      (*tmp)->data = "CALL";
    }

    return tmp;
  }
};