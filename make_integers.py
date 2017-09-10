import sys
import string

SGN = ("u", "s")
LEN = ("64f", "32f", "16f", "8f", "64", "32", "16", "8", "")

def getTypeName (typeSgn, typeLen):
  return "i" + typeSgn + typeLen

def main (args):
  f = open("integers.cpp", "wb")

  f.write("// Generated by make_integers.py\n\n" +
          "#include \"header.hpp\"\n" +
          "#include <climits>\n" +
          "\n" +
          "using core::numeric_limits;\n" +
          "using core::unsign;\n" +
          "using core::check;\n" +
          "\n" +
          "void testIntegers () {")

  for typeSgn in SGN:
    for typeLen in LEN:
      type = getTypeName(typeSgn, typeLen)
      f.write("\n" +
              "  // Test " + type + ".\n" +
              "  {\n")

      trait = {"u": "is_unsigned", "s": "is_signed"}[typeSgn]
      f.write("    size_t bits = numeric_limits<" + type + ">::bits;\n")
      f.write("    check(std::" + trait + "<" + type + ">::value);\n")
      f.write("    check(sizeof(" + type + ") * CHAR_BIT == bits);\n")
      unsignedType = getTypeName("u", typeLen)
      f.write("    check(typeid(" + unsignedType + "), typeid(unsign<" + type + ">(0)));\n")

      if typeLen == "":
        f.write("    check(bits >= numeric_limits<int>::bits); // " + type + " should probably be plain intish\n")
      elif typeLen.endswith("f"):
        f.write("    check(" + typeLen[0:-1] + "U, bits);\n")
      else:
        f.write("    check(bits >= " + typeLen + ");\n")

      if not typeLen.endswith("f"):
        if typeLen == "":
          otherTypeLens = ("",)
        else:
          otherTypeLens = ("64", "32", "16", "8")

        for i in xrange(otherTypeLens.index(typeLen), len(otherTypeLens)):
          f.write("    check(typeid(" + type + "), typeid(static_cast<" + type + ">(0) + static_cast<" + getTypeName(typeSgn, otherTypeLens[i]) + ">(0)));\n")

      f.write("  }\n")

  f.write("}\n")
  f.close()

if __name__=="__main__":
  main([arg.decode(sys.stdin.encoding) for arg in sys.argv[1:]])
