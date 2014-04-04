using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Text.RegularExpressions;

namespace VMAGen
{
    class Program
    {
        static int CountLeadingWhitespace(string t)
        {
            for (int i = 0; i < t.Length; ++i)
            {
                if (!char.IsWhiteSpace(t[i]))
                    return i;
            }
            return t.Length;
        }

        static void Main(string[] args)
        {
            string line;

            bool openCaseBlock = false;

            Console.WriteLine("#pragma pack(push, 1)");

            while ((line = Console.ReadLine()) != null)
            {
                // replace tabs with spaces
                line = line.Replace("\t", "    ");

                string leading_WS = line.Substring(0, CountLeadingWhitespace(line));
                string trimmed_line = line.Trim();

                var matches = Regex.Matches(line, "(VM(A|F)\\([0-9]\\))|(args\\[[0-9]\\])");
                if (matches.Count > 0)
                {
                    Console.WriteLine(leading_WS + "struct __tmp {");
                    Console.WriteLine(leading_WS + "    size_t a0;");

                    int matchIndex = 1;
                    foreach (var match in matches)
                    {
                        string argName = String.Format("a{0}", matchIndex++);

                        string typ = match.ToString().Substring(0, 3);
                        switch (typ)
                        {
                            case "VMA":
                                Console.WriteLine(String.Format("{0}    size_t {1};", leading_WS, argName));
                                break;
                            case "VMF":
                                Console.WriteLine(String.Format("{0}    float {1};", leading_WS, argName));
                                break;
                            case "arg":
                                Console.WriteLine(String.Format("{0}    int {1};", leading_WS, argName));
                                break;
                            default:
                                break;
                        }

                        if (typ == "VMA")
                            line = line.Replace(match.ToString(), String.Format("VM_ArgPtr( p->{0} )", argName));
                        else
                            line = line.Replace(match.ToString(), String.Format("p->{0}", argName));
                    }

                    Console.WriteLine(leading_WS + "};");
                    Console.WriteLine(leading_WS + "const struct __tmp* p = (const struct __tmp*) args;");
                }
                
                if (trimmed_line.StartsWith("case"))
                {
                    // Add a { after this line
                    line += Environment.NewLine + leading_WS + '{';
                    openCaseBlock = true;
                }
                else if (line.Contains("return") || line.Contains("break"))
                {
                    if (openCaseBlock)
                    {
                        // Close the previous case block
                        line = line + Environment.NewLine + leading_WS.Substring(0, Math.Max(0, leading_WS.Length - 4)) + '}';
                    }
                }

                Console.WriteLine(line);
            }

            Console.WriteLine("#pragma pack(pop)");
        }
    }
}
