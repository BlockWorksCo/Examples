
import re
import sys

sourceText  = open(sys.argv[1]).read()
macros      = re.compile('#ifdef\s+PREPROCESSOR(.*?)#endif',re.MULTILINE|re.DOTALL).findall(sourceText)[0]
exec(compile(macros, sys.argv[1],'exec'))
print(Process(sourceText))
