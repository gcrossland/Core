import sys
import string

SGN=["u", "s"]
LEN=["64f", "32f", "16f", "8f", "64", "32", "16", "8", ""]

def main (args):
  f=open("integers.cpp", "wb")

  f.write("// Generated by make_integers.py\n\n" +
          "#include \"header.hpp\"\n" +
          "#include <climits>\n" +
          "\n" +
          "using namespace core;\n" +
          "\n" +
          "void testIntegers () {\n")

  for typeSgn in SGN:
    for typeLen in LEN:
      type = "i" + typeSgn + typeLen
      f.write("\n" +
              "  // Test " + type + ".\n" +
              "  {\n")

      trait = {"u": "is_unsigned", "s": "is_signed"}[typeSgn]
      f.write("    auto bits = numeric_limits<" + type + ">::bits;\n")
      f.write("    check(std::" + trait + "<" + type + ">::value);\n")
      f.write("    check(sizeof(" + type + ") * CHAR_BIT, bits);\n")

      if typeLen == "":
        f.write("    check(bits >= numeric_limits<int>::bits); // " + type + " should probably be plain intish\n")
      elif typeLen.endswith("f"):
        f.write("    check(" + typeLen[0:-1] + ", bits);\n")
      else:
        f.write("    check(bits >= " + typeLen + ");\n")

      f.write("  }\n")

  f.write("}\n")
  f.close()



if __name__=="__main__":
  main([arg.decode(sys.stdin.encoding) for arg in sys.argv[1:]])
